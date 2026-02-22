# KDD Project

Проект по работе с дозиметрами.

##  🚧 Статус
Активный рефакторинг архитектуры и всего-всего

```bash
git clone https://gitlab.com/sophiabob/qt-kdd.git
cd qt-kdd
```


## 🛠️ Сборка
1. Открыть `kdd.pro` в Qt Creator
2. Run qmake → Build → Run


## ⚙️ Требования
- Qt 5.15+
- C++17
- СУБД: PostgreSQL


## 📁 Структура проекта
```
kdd-project/
├── main.cpp                    # Точка входа
├── kdd.pro                     # Файл проекта qmake
├── src/
│   ├── ui/                     # Интерфейс (виджеты, окна)
│   │   ├── mainwindow.*
│   │   ├── welcome.*
│   │   └── widgets/            # Кастомные виджеты
│   ├── models/                 # Модели данных
│   ├── repositories/           # Работа с БД
│   └── helpers/                # Утилиты (namespace)
├── resources/                  # Ресурсы (иконки, переводы)
├── img/                        # Изображения
├── tests/                      # Автотесты
└── build/                      # Папка сборки (игнорируется Git)
```


## 📞 Контакты
Telegram: @sophiabob
