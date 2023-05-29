from ubuntu:latest

ARG USERNAME 

RUN apt-get update
RUN apt-get install git -y
RUN apt-get install openssl -y
RUN apt-get install libssl-dev -y
RUN apt-get install wget -y
RUN apt-get install vim -y
RUN apt-get install g++ -y
RUN apt-get install cmake -y
RUN apt-get install gdb -y

RUN useradd -m -s /bin/bash $USERNAME
USER $USERNAME
WORKDIR /home/$USERNAME