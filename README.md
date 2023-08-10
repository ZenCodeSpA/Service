# System

## Documentation

### CLI

#### HTTP

```bash
./System --run \
         --http=on \
         --http_port=443 \
         --http_threads=5 \
         --http_directory=assets
```

### Queue

```bash
./System --run \
         --queue=on \
         --queue_threads=10
```

### HTTP + Queue

```bash
./System --run \
         --http=on \
         --http_port=443 \
         --http_threads=5 \
         --http_directory=assets \
         --queue=on \
         --queue_threads=10
```

### Available Options

```bash
./System --help
```