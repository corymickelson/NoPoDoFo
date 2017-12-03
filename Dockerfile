FROM node:8.9.1-alpine

RUN apk update && \
    apk upgrade && \
    apk add tiff-dev libpng-dev jpeg-dev fontconfig-dev freetype-dev libidn-dev openssl-dev zlib-dev cmake make gcc g++ git python2

# COPY binding.gyp /home/npdf/
# COPY CMakeLists.txt /home/npdf/
# COPY package.json /home/npdf/
# COPY package-lock.json /home/npdf/
# COPY tsconfig.json /home/npdf/
# COPY typings.json /home/npdf/
# COPY src /home/npdf/src
# COPY lib/* /home/npdf/lib/
# COPY test-documents/* /home/npdf/test-documents/

ADD alpine-install-deps.sh /
RUN /alpine-install-deps.sh

VOLUME /home/src
WORKDIR /home/src

EXPOSE 22
#CMD [ "/usr/sbin/sshd", "-D" ]