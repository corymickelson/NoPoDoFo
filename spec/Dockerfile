FROM node:10.5.0-alpine

RUN apk update && \
    apk upgrade && \
    apk add tiff-dev libpng-dev jpeg-dev fontconfig-dev freetype-dev libidn-dev openssl-dev zlib-dev cmake make gcc g++ git python2 ttf-freefont boost-dev

ADD alpine-install-deps.sh /
RUN /alpine-install-deps.sh

VOLUME /home/src
WORKDIR /home/src

ENV NODE_PATH /usr/local/lib/node_modules

EXPOSE 22