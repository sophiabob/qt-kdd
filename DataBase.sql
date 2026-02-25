CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- Авторизация
    login TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,

    -- ФИО
    name_0 TEXT,  -- Фамилия
    name_1 TEXT,  -- Имя
    name_2 TEXT,  -- Отчество

    -- Личные данные
    snils INTEGER UNIQUE,
    gender TEXT,
    birthday DATETIME,

    -- Работа
    role TEXT DEFAULT 'user',
    tab_num INTEGER,
    department TEXT,

    -- Доступы
    card_id TEXT,
    code TEXT,
    block TEXT,  -- Причина блокировки (NULL = активен)

    -- Технические ID
    set_ID INTEGER,
    kas_ID INTEGER,
    mesh_ID INTEGER,
    doz_tld_id TEXT,

    -- Дозиметрия
    cell_date DATETIME,
    dose_year REAL,
    dose_year_now REAL,
    dose_year_now_ppd REAL,

    -- Системные поля
    last_update DATETIME DEFAULT CURRENT_TIMESTAMP,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Индексы для ускорения поиска
CREATE INDEX IF NOT EXISTS idx_users_login ON users(login);
CREATE INDEX IF NOT EXISTS idx_users_snils ON users(snils);
CREATE INDEX IF NOT EXISTS idx_users_department ON users(department);


