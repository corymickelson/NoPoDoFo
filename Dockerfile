FROM node:8.9.1-alpine

RUN apk update && \
    apk upgrade && \
    apk add tiff-dev libpng-dev jpeg-dev fontconfig-dev freetype-dev libidn-dev openssl-dev zlib-dev cmake make gcc g++ git python2

ADD alpine-install-deps.sh /
RUN /alpine-install-deps.sh

