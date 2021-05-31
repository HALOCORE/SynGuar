FROM ubuntu:20.04

LABEL maintainer="Bo Wang (bo_wang@u.nus.edu)"

RUN apt-get update && apt-get -y upgrade
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y build-essential python3.8 python3.8-dev python3-pip m4 cmake
RUN pip3 install Flask==1.1.2 \
      Flask-Cors==3.0.10 \
      Flask-RESTful==0.3.8 \
      psutil==5.8.0 \
      requests==2.18.4
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y vim wget
RUN cd /tmp && wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz && \
        tar -xf ./gmp-6.2.1.tar.xz && \
        cd ./gmp-6.2.1 && \
        ./configure --enable-cxx --prefix=/home/synguar/SynGuarAll/StrSTUN-synthesizer/dependencies/libs/_gmp621 && \
        make install
RUN cd /tmp && wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb && \
    dpkg -i packages-microsoft-prod.deb && apt-get update && \
    apt-get install -y apt-transport-https && \
    apt-get update && \
    apt-get install -y dotnet-sdk-5.0 dotnet-runtime-3.1

# copy StrSTUN
COPY ./StrSTUN-synthesizer /home/synguar/SynGuarAll/StrSTUN-synthesizer

# compile StrSTUN
RUN cd /home/synguar/SynGuarAll/ && mkdir build-strstun && \
        cd build-strstun && \
        cmake -DCMAKE_BUILD_TYPE=Release ../StrSTUN-synthesizer/ && \
        make

# copy StrPROSE
COPY ./StrPROSE-synthesizer /home/synguar/SynGuarAll/StrPROSE-synthesizer

# compile StrPROSE
RUN cd /home/synguar/SynGuarAll/StrPROSE-synthesizer/StrPROSE && dotnet build

COPY ./SynGuar /home/synguar/SynGuarAll/SynGuar

# https://docs.docker.com/engine/reference/builder/#expose
EXPOSE 5261 5262 5263 5265

WORKDIR /home/synguar/SynGuarAll/

CMD exec /bin/bash -c "sleep infinity & wait"