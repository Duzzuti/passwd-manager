from ubuntu:latest

ARG USERNAME 

RUN apt-get update && apt-get install -y git openssl libssl-dev g++ cmake gdb locales

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
