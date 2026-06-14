import os
import zipfile
import urllib.request
import numpy as np
import pandas as pd
from surprise import Dataset, Reader
from typing import Tuple, Dict, Optional, List
from .config import Config
from . import setup_logger

logger = setup_logger('data_loader')


class DataLoader:
    def __init__(self):
        self.ratings_df: Optional[pd.DataFrame] = None
        self.movies_df: Optional[pd.DataFrame] = None
        self.links_df: Optional[pd.DataFrame] = None
        self.new_interactions_df: Optional[pd.DataFrame] = None

        self.user_id_map: Dict[int, int] = {}
        self.movie_id_map: Dict[int, int] = {}
        self.reverse_user_map: Dict[int, int] = {}
        self.reverse_movie_map: Dict[int, int] = {}

        self.movie_titles: Dict[int, str] = {}
        self.movie_genres: Dict[int, List[str]] = {}
        self.movie_imdb_ids: Dict[int, str] = {}

        self.user_rating_counts: Dict[int, int] = {}
        self.movie_rating_counts: Dict[int, int] = {}
        self.popular_movies: List[int] = []

    def download_movielens(self, dataset_size: str = 'ml-latest-small') -> None:
        url = f'http://files.grouplens.org/datasets/movielens/{dataset_size}.zip'
        zip_path = os.path.join(Config.DATA_DIR, f'{dataset_size}.zip')

        if os.path.exists(Config.RATINGS_FILE) and os.path.exists(Config.MOVIES_FILE):
            logger.info('MovieLens data already exists, skipping download.')
            return

        logger.info(f'Downloading MovieLens {dataset_size} dataset...')
        os.makedirs(Config.DATA_DIR, exist_ok=True)

        try:
            urllib.request.urlretrieve(url, zip_path)
            logger.info('Download complete. Extracting files...')

            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                for file_info in zip_ref.infolist():
                    filename = os.path.basename(file_info.filename)
                    if filename in ['ratings.csv', 'movies.csv', 'links.csv']:
                        target_path = os.path.join(Config.DATA_DIR, filename)
                        with zip_ref.open(file_info) as source, open(target_path, 'wb') as target:
                            target.write(source.read())

            os.remove(zip_path)
            logger.info('MovieLens dataset prepared successfully.')
        except Exception as e:
            logger.error(f'Failed to download MovieLens dataset: {e}')
            raise

    def load_data(self, generate_sample: bool = True) -> None:
        if not os.path.exists(Config.RATINGS_FILE):
            if generate_sample:
                logger.warning('Data files not found, generating synthetic sample data...')
                self._generate_sample_data()
            else:
                self.download_movielens()

        logger.info('Loading ratings data...')
        self.ratings_df = pd.read_csv(Config.RATINGS_FILE)
        self.ratings_df.columns = [c.lower() for c in self.ratings_df.columns]
        self.ratings_df = self.ratings_df[['userid', 'movieid', 'rating', 'timestamp']]
        self.ratings_df['rating'] = self.ratings_df['rating'].astype(float)

        logger.info('Loading movies data...')
        self.movies_df = pd.read_csv(Config.MOVIES_FILE)
        self.movies_df.columns = [c.lower() for c in self.movies_df.columns]
        self.movies_df = self.movies_df[['movieid', 'title', 'genres']]

        if os.path.exists(Config.LINKS_FILE):
            logger.info('Loading links data...')
            self.links_df = pd.read_csv(Config.LINKS_FILE)
            self.links_df.columns = [c.lower() for c in self.links_df.columns]
            self.links_df = self.links_df[['movieid', 'imdbid', 'tmdbid']]
        else:
            self.links_df = pd.DataFrame(columns=['movieid', 'imdbid', 'tmdbid'])

        self._build_mappings()
        self._compute_statistics()
        self._load_new_interactions()

        logger.info(f'Data loaded: {len(self.ratings_df)} ratings, '
                    f'{len(self.movies_df)} movies, {self.ratings_df["userid"].nunique()} users')

    def _generate_sample_data(self) -> None:
        np.random.seed(42)
        n_users = 100
        n_movies = 50
        n_ratings = 5000

        user_ids = np.arange(1, n_users + 1)
        movie_ids = np.arange(1, n_movies + 1)

        ratings_data = []
        for _ in range(n_ratings):
            uid = np.random.choice(user_ids)
            mid = np.random.choice(movie_ids)
            rating = np.random.choice([0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0],
                                      p=[0.05, 0.05, 0.05, 0.1, 0.1, 0.15, 0.2, 0.15, 0.1, 0.05])
            ts = int(1609459200 + np.random.randint(0, 365 * 24 * 3600))
            ratings_data.append([uid, mid, rating, ts])

        ratings_df = pd.DataFrame(ratings_data, columns=['userId', 'movieId', 'rating', 'timestamp'])
        ratings_df.to_csv(Config.RATINGS_FILE, index=False)

        genres_pool = ['Action', 'Adventure', 'Animation', 'Children', 'Comedy', 'Crime',
                       'Documentary', 'Drama', 'Fantasy', 'Film-Noir', 'Horror', 'Musical',
                       'Mystery', 'Romance', 'Sci-Fi', 'Thriller', 'War', 'Western']
        directors_pool = ['Christopher Nolan', 'Steven Spielberg', 'Martin Scorsese',
                          'Quentin Tarantino', 'James Cameron', 'Peter Jackson',
                          'Ridley Scott', 'David Fincher', 'Christopher Nolan', 'Clint Eastwood']

        movies_data = []
        for i in range(n_movies):
            mid = i + 1
            title = f'Sample Movie {mid} ({2000 + i % 25})'
            n_genres = np.random.randint(1, 4)
            selected_genres = np.random.choice(genres_pool, size=n_genres, replace=False)
            genres = '|'.join(selected_genres)
            movies_data.append([mid, title, genres])

        movies_df = pd.DataFrame(movies_data, columns=['movieId', 'title', 'genres'])

        director_col = np.random.choice(directors_pool, size=n_movies)
        movies_df['director'] = director_col

        movies_df.to_csv(Config.MOVIES_FILE, index=False)

        links_data = [[i + 1, f'tt{i + 1:07d}', i + 1] for i in range(n_movies)]
        links_df = pd.DataFrame(links_data, columns=['movieId', 'imdbId', 'tmdbId'])
        links_df.to_csv(Config.LINKS_FILE, index=False)

        logger.info('Synthetic sample data generated.')

    def _build_mappings(self) -> None:
        unique_users = sorted(self.ratings_df['userid'].unique())
        unique_movies = sorted(self.ratings_df['movieid'].unique())

        self.user_id_map = {uid: idx for idx, uid in enumerate(unique_users)}
        self.reverse_user_map = {idx: uid for uid, idx in self.user_id_map.items()}

        all_movies = sorted(self.movies_df['movieid'].unique())
        self.movie_id_map = {mid: idx for idx, mid in enumerate(all_movies)}
        self.reverse_movie_map = {idx: mid for mid, idx in self.movie_id_map.items()}

        for _, row in self.movies_df.iterrows():
            mid = row['movieid']
            self.movie_titles[mid] = row['title']
            self.movie_genres[mid] = row['genres'].split('|') if pd.notna(row['genres']) else []

        if not self.links_df.empty:
            for _, row in self.links_df.iterrows():
                mid = row['movieid']
                imdb_id = row['imdbid']
                if pd.notna(imdb_id):
                    self.movie_imdb_ids[mid] = f'tt{int(imdb_id):07d}'

    def _compute_statistics(self) -> None:
        self.user_rating_counts = self.ratings_df['userid'].value_counts().to_dict()
        self.movie_rating_counts = self.ratings_df['movieid'].value_counts().to_dict()

        movie_stats = self.ratings_df.groupby('movieid').agg(
            avg_rating=('rating', 'mean'),
            count=('rating', 'count')
        ).reset_index()
        min_count = max(1, int(len(self.ratings_df) * 0.005))
        movie_stats = movie_stats[movie_stats['count'] >= min_count]
        movie_stats = movie_stats.sort_values(['avg_rating', 'count'], ascending=False)
        self.popular_movies = movie_stats['movieid'].head(100).tolist()

    def _load_new_interactions(self) -> None:
        if os.path.exists(Config.NEW_INTERACTIONS_FILE):
            try:
                self.new_interactions_df = pd.read_csv(Config.NEW_INTERACTIONS_FILE)
                self.new_interactions_df.columns = [c.lower() for c in self.new_interactions_df.columns]
                logger.info(f'Loaded {len(self.new_interactions_df)} new pending interactions.')
            except Exception as e:
                logger.warning(f'Failed to load new interactions: {e}')
                self.new_interactions_df = pd.DataFrame(
                    columns=['userid', 'movieid', 'rating', 'timestamp']
                )
        else:
            self.new_interactions_df = pd.DataFrame(
                columns=['userid', 'movieid', 'rating', 'timestamp']
            )

    def add_interaction(self, user_id: int, movie_id: int, rating: float,
                        timestamp: Optional[int] = None) -> None:
        if timestamp is None:
            import time
            timestamp = int(time.time())

        new_row = pd.DataFrame([[user_id, movie_id, rating, timestamp]],
                               columns=['userid', 'movieid', 'rating', 'timestamp'])

        self.new_interactions_df = pd.concat(
            [self.new_interactions_df, new_row], ignore_index=True
        )

        if self.user_rating_counts:
            self.user_rating_counts[user_id] = self.user_rating_counts.get(user_id, 0) + 1
            self.movie_rating_counts[movie_id] = self.movie_rating_counts.get(movie_id, 0) + 1

    def save_new_interactions(self) -> None:
        self.new_interactions_df.to_csv(Config.NEW_INTERACTIONS_FILE, index=False)
        logger.info(f'Saved {len(self.new_interactions_df)} pending interactions.')

    def get_surprise_dataset(self, use_new_interactions: bool = True) -> Dataset:
        df = self.ratings_df.copy()
        if use_new_interactions and self.new_interactions_df is not None and len(self.new_interactions_df) > 0:
            df = pd.concat([df, self.new_interactions_df], ignore_index=True)

        reader = Reader(rating_scale=(df['rating'].min(), df['rating'].max()))
        data = Dataset.load_from_df(df[['userid', 'movieid', 'rating']], reader)
        return data

    def get_ratings_matrix(self) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        n_users = len(self.user_id_map)
        n_movies = len(self.movie_id_map)

        users = []
        items = []
        ratings = []

        for _, row in self.ratings_df.iterrows():
            u_idx = self.user_id_map.get(row['userid'])
            i_idx = self.movie_id_map.get(row['movieid'])
            if u_idx is not None and i_idx is not None:
                users.append(u_idx)
                items.append(i_idx)
                ratings.append(row['rating'])

        if self.new_interactions_df is not None and len(self.new_interactions_df) > 0:
            for _, row in self.new_interactions_df.iterrows():
                if row['userid'] not in self.user_id_map:
                    new_idx = len(self.user_id_map)
                    self.user_id_map[row['userid']] = new_idx
                    self.reverse_user_map[new_idx] = row['userid']
                    n_users += 1
                if row['movieid'] not in self.movie_id_map:
                    new_idx = len(self.movie_id_map)
                    self.movie_id_map[row['movieid']] = new_idx
                    self.reverse_movie_map[new_idx] = row['movieid']
                    n_movies += 1
                u_idx = self.user_id_map[row['userid']]
                i_idx = self.movie_id_map[row['movieid']]
                users.append(u_idx)
                items.append(i_idx)
                ratings.append(row['rating'])

        return np.array(users, dtype=np.int32), np.array(items, dtype=np.int32), np.array(ratings, dtype=np.float32)

    def is_new_user(self, user_id: int) -> bool:
        count = self.user_rating_counts.get(user_id, 0)
        return count < Config.NEW_USER_THRESHOLD

    def is_new_movie(self, movie_id: int) -> bool:
        count = self.movie_rating_counts.get(movie_id, 0)
        return count < Config.NEW_ITEM_THRESHOLD

    def get_user_watched_movies(self, user_id: int) -> Dict[int, float]:
        watched = {}
        df = self.ratings_df[self.ratings_df['userid'] == user_id]
        for _, row in df.iterrows():
            watched[row['movieid']] = row['rating']

        if self.new_interactions_df is not None and len(self.new_interactions_df) > 0:
            df_new = self.new_interactions_df[self.new_interactions_df['userid'] == user_id]
            for _, row in df_new.iterrows():
                watched[row['movieid']] = row['rating']
        return watched

    def get_movie_avg_rating(self, movie_id: int) -> float:
        df = self.ratings_df[self.ratings_df['movieid'] == movie_id]
        if len(df) == 0:
            return 3.0
        return float(df['rating'].mean())

    def get_movies_by_genre(self, genre: str) -> List[int]:
        return [mid for mid, genres in self.movie_genres.items() if genre in genres]

    def merge_new_interactions(self) -> int:
        if self.new_interactions_df is None or len(self.new_interactions_df) == 0:
            logger.info('No new interactions to merge.')
            return 0

        n = len(self.new_interactions_df)
        self.ratings_df = pd.concat(
            [self.ratings_df, self.new_interactions_df], ignore_index=True
        )
        self.new_interactions_df = pd.DataFrame(columns=['userid', 'movieid', 'rating', 'timestamp'])

        if os.path.exists(Config.NEW_INTERACTIONS_FILE):
            os.remove(Config.NEW_INTERACTIONS_FILE)

        self._build_mappings()
        self._compute_statistics()
        logger.info(f'Merged {n} new interactions into main dataset.')
        return n
