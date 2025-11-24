FROM ubuntu:20.04

# Install dependencies
RUN apt update && apt install -y \
    build-essential \
    libpq-dev \
    postgresql \
    curl

# Copy your server binary (build it first on host)
COPY server /app/server
COPY load_generator /app/load_generator

WORKDIR /app
CMD ["./server", "8080"]
