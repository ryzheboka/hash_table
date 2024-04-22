FROM gcc:latest

COPY . /hash_table

WORKDIR /hash_table/

ENV DISPLAY=:0

RUN make client

RUN make server
