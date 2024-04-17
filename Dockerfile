FROM gcc:latest

COPY . /hash_table

WORKDIR /hash_table/

RUN make server

RUN make client