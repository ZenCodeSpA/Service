# Service

## Documentation

### CLI

### Usage

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

### Details

```bash
./service --help
```