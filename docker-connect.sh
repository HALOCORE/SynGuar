echo "
NOTICE:

- 3 folders are copied into docker image during build time:
    - StrPROSE-synthesizer
    - StrSTUN-synthesizer
    - SynGuar

- 1 folder is created during build time:
    - build-strstun

- 2 folders are mounted while running:
    - benchmark
    - outputs

"
docker exec -it synguar-all /bin/bash