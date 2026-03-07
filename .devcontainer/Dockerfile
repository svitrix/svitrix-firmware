FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        python3-venv \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --break-system-packages --upgrade platformio
