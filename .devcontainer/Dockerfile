FROM ubuntu:25.10

# Update the package database
RUN apt clean
RUN apt update

# Install common tools
RUN apt install -y python3-minimal python3-pip pipx git less vim sudo cmake make g++-15 clang-format clangd-20

# Install additional tools and libraries
RUN apt install -y nodejs npm cargo

RUN apt install -y valgrind

# Cleanup
RUN apt autoremove -y \
    && apt autoclean \
    && rm -rf /var/lib/apt/lists/* \
    && rm -rf /var/cache/apt/* \
    && rm -rf /tmp/*

# Update  gcc/g++ version to the latest ones
RUN ln -fs /usr/bin/g++-15 /usr/bin/g++
RUN ln -fs /usr/bin/gcc-15 /usr/bin/gcc

# Установка rustup и последней стабильной версии Rust
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

RUN cargo install tree-sitter-cli --version 0.24.7 --root /usr/local --locked
ENV PATH="/usr/local/bin:/root/.cargo/bin:${PATH}"
WORKDIR ..
RUN git clone --branch v0.25.0 --depth 1 https://github.com/tree-sitter/tree-sitter-python
WORKDIR tree-sitter-python/
RUN ls -la
RUN tree-sitter init-config
RUN tree-sitter generate
RUN gcc -shared -o libtree-sitter-python.so -Isrc src/parser.c -fPIC
RUN sudo mkdir -p ~/.tree-sitter/bin
RUN sudo ln -s $(pwd)/libtree-sitter-python.so ~/.tree-sitter/bin/

WORKDIR ..
RUN echo '{"parser-directories":["/"]}' > /root/.config/tree-sitter/config.json

RUN chmod 777 /root/.config/tree-sitter/config.json
RUN chmod 755 /root
RUN chmod 755 /root/.config
RUN chmod 755 /root/.config/tree-sitter
RUN chmod 644 /root/.config/tree-sitter/config.json


# Create a non-root user 'dev'
RUN useradd -ms /bin/bash dev \
    && echo "dev ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/dev \
    && chmod 0440 /etc/sudoers.d/dev

# Switch to the 'dev' user
USER dev

RUN ls /root/.config/tree-sitter/config.json 

RUN git config --global core.editor code

RUN pipx install conan
ENV PATH="/home/dev/.local/bin:${PATH}"
RUN echo "conan installed" && \
    conan --version
RUN pipx ensurepath
