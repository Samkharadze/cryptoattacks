FROM alpine:3.10.2

LABEL maintainer="giorgisamh1771@gmail.com"



RUN apk update && \
    apk add linux-headers \
            musl-dev \
            build-base \
            gcc \
            git \
            g++ \
            make \
            cmake \
            bash \
            && \

    cd / && \
    rm -rf ${HOME}/.cache && \
    rm -rf /var/cache/apk/*

VOLUME /opt/builder
WORKDIR /opt/builder
