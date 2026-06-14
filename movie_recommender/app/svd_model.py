import os
import joblib
import numpy as np
from typing import List, Dict, Tuple, Optional
from surprise import SVD, accuracy
from surprise.model_selection import cross_validate, train_test_split
from .config import Config
from .data_loader import DataLoader
from . import setup_logger

logger = setup_logger('svd_model')


class SVDRecommender:
    def __init__(self, data_loader: DataLoader):
        self.data_loader = data_loader
        self.model: Optional[SVD] = None
        self.trainset = None
        self.is_fitted = False

    def build_model(self) -> SVD:
        logger.info(f'Building SVD model with n_factors={Config.SVD_N_FACTORS}, '
                    f'n_epochs={Config.SVD_N_EPOCHS}')
        self.model = SVD(
            n_factors=Config.SVD_N_FACTORS,
            n_epochs=Config.SVD_N_EPOCHS,
            lr_all=Config.SVD_LR_ALL,
            reg_all=Config.SVD_REG_ALL,
            random_state=42
        )
        return self.model

    def train(self, use_cross_validation: bool = True) -> Dict:
        logger.info('Starting SVD model training...')
        dataset = self.data_loader.get_surprise_dataset()
        self.trainset = dataset.build_full_trainset()

        if self.model is None:
            self.build_model()

        metrics = {}

        if use_cross_validation:
            logger.info('Running 5-fold cross validation...')
            cv_results = cross_validate(
                self.model, dataset,
                measures=['RMSE', 'MAE'],
                cv=5,
                verbose=True,
                n_jobs=-1
            )
            metrics['cv_rmse'] = float(np.mean(cv_results['test_rmse']))
            metrics['cv_mae'] = float(np.mean(cv_results['test_mae']))
            metrics['cv_fit_time'] = float(np.mean(cv_results['fit_time']))
            metrics['cv_test_time'] = float(np.mean(cv_results['test_time']))
            logger.info(f'CV Results - RMSE: {metrics["cv_rmse"]:.4f}, MAE: {metrics["cv_mae"]:.4f}')

        trainset, testset = train_test_split(dataset, test_size=0.2, random_state=42)
        self.model.fit(trainset)
        predictions = self.model.test(testset)
        metrics['holdout_rmse'] = float(accuracy.rmse(predictions, verbose=False))
        metrics['holdout_mae'] = float(accuracy.mae(predictions, verbose=False))
        logger.info(f'Holdout Results - RMSE: {metrics["holdout_rmse"]:.4f}, '
                    f'MAE: {metrics["holdout_mae"]:.4f}')

        self.model.fit(self.trainset)
        self.is_fitted = True
        logger.info('SVD model training complete.')
        return metrics

    def incremental_train(self, new_interactions_only: bool = True) -> int:
        if self.model is None:
            logger.warning('No existing model, performing full training instead.')
            self.train(use_cross_validation=False)
            return 0

        if new_interactions_only:
            n_new = self.data_loader.merge_new_interactions()
        else:
            n_new = 0

        logger.info(f'Incrementally updating SVD model with {n_new} new interactions...')
        dataset = self.data_loader.get_surprise_dataset()
        self.trainset = dataset.build_full_trainset()

        self.model.n_epochs = max(5, Config.SVD_N_EPOCHS // 3)
        self.model.fit(self.trainset)
        self.model.n_epochs = Config.SVD_N_EPOCHS
        self.is_fitted = True
        logger.info('SVD incremental update complete.')
        return n_new

    def predict_rating(self, user_id: int, movie_id: int) -> float:
        if not self.is_fitted:
            return self.data_loader.get_movie_avg_rating(movie_id)

        try:
            pred = self.model.predict(user_id, movie_id)
            return float(pred.est)
        except Exception:
            return self.data_loader.get_movie_avg_rating(movie_id)

    def recommend(self, user_id: int, top_n: int = 10,
                  exclude_watched: bool = True) -> List[Tuple[int, float]]:
        if not self.is_fitted:
            return []

        watched = self.data_loader.get_user_watched_movies(user_id) if exclude_watched else {}
        all_movie_ids = list(self.data_loader.movie_id_map.keys())

        candidates = [mid for mid in all_movie_ids if mid not in watched]
        if len(candidates) == 0:
            candidates = all_movie_ids

        predictions = []
        for mid in candidates:
            pred = self.predict_rating(user_id, mid)
            predictions.append((mid, pred))

        predictions.sort(key=lambda x: x[1], reverse=True)
        return predictions[:top_n]

    def batch_predict(self, user_id: int, movie_ids: List[int]) -> Dict[int, float]:
        results = {}
        for mid in movie_ids:
            results[mid] = self.predict_rating(user_id, mid)
        return results

    def save(self, path: Optional[str] = None) -> str:
        if path is None:
            path = Config.SVD_MODEL_PATH
        os.makedirs(os.path.dirname(path), exist_ok=True)
        state = {
            'model': self.model,
            'trainset': self.trainset,
            'is_fitted': self.is_fitted
        }
        joblib.dump(state, path)
        logger.info(f'SVD model saved to {path}')
        return path

    def load(self, path: Optional[str] = None) -> bool:
        if path is None:
            path = Config.SVD_MODEL_PATH
        if not os.path.exists(path):
            logger.warning(f'SVD model file not found: {path}')
            return False
        try:
            state = joblib.load(path)
            self.model = state['model']
            self.trainset = state['trainset']
            self.is_fitted = state['is_fitted']
            logger.info(f'SVD model loaded from {path}')
            return True
        except Exception as e:
            logger.error(f'Failed to load SVD model: {e}')
            return False

    def get_similar_users(self, user_id: int, top_n: int = 10) -> List[Tuple[int, float]]:
        if not self.is_fitted or not hasattr(self.model, 'pu'):
            return []

        user_idx = self.data_loader.user_id_map.get(user_id)
        if user_idx is None or user_idx >= len(self.model.pu):
            return []

        user_factors = self.model.pu[user_idx]
        similarities = []

        for other_uid, other_idx in self.data_loader.user_id_map.items():
            if other_uid == user_id:
                continue
            if other_idx >= len(self.model.pu):
                continue
            other_factors = self.model.pu[other_idx]
            sim = self._cosine_similarity(user_factors, other_factors)
            similarities.append((other_uid, float(sim)))

        similarities.sort(key=lambda x: x[1], reverse=True)
        return similarities[:top_n]

    def get_user_high_rated_movies(self, user_id: int, threshold: float = 4.0) -> List[int]:
        watched = self.data_loader.get_user_watched_movies(user_id)
        return [mid for mid, rating in watched.items() if rating >= threshold]

    @staticmethod
    def _cosine_similarity(a: np.ndarray, b: np.ndarray) -> float:
        norm_a = np.linalg.norm(a)
        norm_b = np.linalg.norm(b)
        if norm_a == 0 or norm_b == 0:
            return 0.0
        return float(np.dot(a, b) / (norm_a * norm_b))
