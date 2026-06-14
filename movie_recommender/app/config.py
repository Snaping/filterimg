import os
from dotenv import load_dotenv

load_dotenv()

class Config:
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))
    ROOT_DIR = os.path.dirname(BASE_DIR)

    MODEL_DIR = os.path.join(ROOT_DIR, os.getenv('MODEL_DIR', 'models'))
    DATA_DIR = os.path.join(ROOT_DIR, os.getenv('DATA_DIR', 'data'))
    LOG_DIR = os.path.join(ROOT_DIR, os.getenv('LOG_DIR', 'logs'))

    for d in [MODEL_DIR, DATA_DIR, LOG_DIR]:
        os.makedirs(d, exist_ok=True)

    OMDB_API_KEY = os.getenv('OMDB_API_KEY', '')
    OMDB_BASE_URL = 'http://www.omdbapi.com/'

    CF_WEIGHT = float(os.getenv('CF_WEIGHT', 0.6))
    CONTENT_WEIGHT = float(os.getenv('CONTENT_WEIGHT', 0.4))

    SVD_N_FACTORS = int(os.getenv('SVD_N_FACTORS', 100))
    SVD_N_EPOCHS = int(os.getenv('SVD_N_EPOCHS', 50))
    SVD_LR_ALL = float(os.getenv('SVD_LR_ALL', 0.005))
    SVD_REG_ALL = float(os.getenv('SVD_REG_ALL', 0.02))

    UPDATE_INTERVAL_HOURS = int(os.getenv('UPDATE_INTERVAL_HOURS', 2))
    TOP_N_DEFAULT = int(os.getenv('TOP_N_DEFAULT', 10))

    FLASK_HOST = os.getenv('HOST', '0.0.0.0')
    FLASK_PORT = int(os.getenv('PORT', 5000))
    FLASK_DEBUG = os.getenv('FLASK_DEBUG', 'True').lower() == 'true'

    NEW_USER_THRESHOLD = 5
    NEW_ITEM_THRESHOLD = 3
    COLD_START_POPULAR_N = 50
    COLD_START_EXPLORE_RATIO = 0.2

    SVD_MODEL_PATH = os.path.join(MODEL_DIR, 'svd_model.pkl')
    USER_CF_MODEL_PATH = os.path.join(MODEL_DIR, 'user_cf_model.pkl')
    CONTENT_MODEL_PATH = os.path.join(MODEL_DIR, 'content_model.pkl')
    DATA_STATE_PATH = os.path.join(MODEL_DIR, 'data_state.pkl')

    RATINGS_FILE = os.path.join(DATA_DIR, 'ratings.csv')
    MOVIES_FILE = os.path.join(DATA_DIR, 'movies.csv')
    LINKS_FILE = os.path.join(DATA_DIR, 'links.csv')
    NEW_INTERACTIONS_FILE = os.path.join(DATA_DIR, 'new_interactions.csv')
