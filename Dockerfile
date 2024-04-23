FROM gcc:latest

COPY . /hash_table

WORKDIR /hash_table/

RUN make client

RUN make server
