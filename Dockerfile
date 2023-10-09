from ubuntu:latest

ARG USERNAME 

RUN apt-get update && apt-get install sudo -y
RUN apt-get update && apt-get install wget -y
RUN apt-get update && apt-get install git -y
RUN apt-get update && apt-get install openssl -y
RUN apt-get update && apt-get install libssl-dev -y
RUN apt-get update && apt-get install g++ -y
RUN apt-get update && apt-get install cmake -y
RUN apt-get update && apt-get install gdb -y
RUN apt-get update && apt-get install locales -y

#CUDA

RUN wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
RUN sudo dpkg -i cuda-keyring_1.1-1_all.deb
RUN sudo apt-get update && sudo apt-get install cuda -y



# Change locale here to your preferred locale:
# Run `locale -a` in your machine to list possible locales
RUN locale-gen en_US.UTF-8  
ENV LANG en_US.UTF-8  
ENV LANGUAGE en_US:en  
ENV LC_ALL en_US.UTF-8

RUN useradd -m -s /bin/bash $USERNAME
USER $USERNAME
WORKDIR /home/$USERNAME
RUN git clone --depth=1 https://github.com/Bash-it/bash-it.git /home/$USERNAME/.bash_it
RUN /home/$USERNAME/.bash_it/install.sh --silent