./docker-stop.sh
docker run --name synguar-all -d -t \
    --mount type=bind,source=$PWD/outputs,target=/home/synguar/SynGuarAll/outputs \
    --mount type=bind,source=$PWD/benchmark,target=/home/synguar/SynGuarAll/benchmark \
    -p 5261:5261/tcp \
    -p 5262:5262/tcp \
    -p 5263:5263/tcp \
    -p 5265:5265/tcp \
    synguar:v1
docker ps
echo "# To stop running the container: ./docker-stop.sh"