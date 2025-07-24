#!/bin/bash

# Цвета ANSI
GREEN="\033[0;32m"
NC="\033[0m" # Без цвета (normal color)

# --- Конфигурация Docker ---
DOCKERFILE_PATH="docker/Dockerfile" # Путь к вашему Dockerfile
IMAGE_NAME="my_project_image"       # Имя Docker-образа
CONTAINER_NAME="my_project_container" # Имя Docker-контейнера

# --- Сборка Docker образа ---
echo "🔨 Сборка Docker образа '$IMAGE_NAME' из $DOCKERFILE_PATH..."
docker build -t "$IMAGE_NAME" -f "$DOCKERFILE_PATH" . || {
    echo "❌ Ошибка сборки Docker образа. Проверьте ваш Dockerfile."
    exit 1
}

# --- Запуск Docker контейнера ---
echo -e "${GREEN}🚀 Запуск Docker контейнера '$CONTAINER_NAME'...${NC}"
docker run --rm -it --name "$CONTAINER_NAME" "$IMAGE_NAME" || {
    echo "❌ Ошибка запуска Docker контейнера. Проверьте CMD в вашем Dockerfile или наличие исполняемого файла."
    exit 1
}

echo -e "${GREEN}✅ Контейнер завершил работу.${NC}"

