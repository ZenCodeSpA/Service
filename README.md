# Service

## Abstract

Micro optimization

## Documentation

### CLI

#### HTTP

```bash
./service --run \
         --http=on \
         --http_port=443 \
         --http_threads=5 \
         --http_directory=assets
```

### Queue

```bash
./service --run \
         --queue=on \
         --queue_threads=10
```

### HTTP + Queue

```bash
./service --run \
         --http=on \
         --http_port=443 \
         --http_threads=5 \
         --http_directory=assets \
         --queue=on \
         --queue_threads=10 \
         --queue_wait_timeout=5000000 \
         --queue_reserved_slots=2048
```

### Available Options

```bash
./service --help
```