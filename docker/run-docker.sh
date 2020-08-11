# NOTE: run this from dir above docker!
docker run -it --rm \
    -v $(pwd):/home/nmsac/nmsac \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -e DISPLAY=$DISPLAY \
    --name nmsac-c \
    --net host \
    --privileged \
    $1 \
    jdinius/nmsac:latest
