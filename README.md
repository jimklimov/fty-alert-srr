# fty-alert-srr

`fty-alert-srr` agent is in charge of performin SRR operations on alert configuration.

## Features
- save alert configuration
- restore alert configuration
- reset alert configuration (to be implemented)

## How to build

```bash
cd build
cmake ..
make
```

## How to run the agent

```bash
./fty-alert-srr
```

## Dependencies
`fty-common-messagebus`
`fty-srr`
