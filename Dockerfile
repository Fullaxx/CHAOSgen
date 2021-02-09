# ------------------------------------------------------------------------------
# Install build tools and compile CHAOSgen
FROM ubuntu:focal AS build
ADD . /code
RUN apt-get update && \
	apt-get install -y build-essential libgcrypt20-dev && \
	cd /code && ./compile.sh

# ------------------------------------------------------------------------------
# Pull base image
FROM ubuntu:focal
MAINTAINER Brett Kuskie <fullaxx@gmail.com>

# ------------------------------------------------------------------------------
# Set environment variables
ENV DEBIAN_FRONTEND noninteractive

# ------------------------------------------------------------------------------
# Install dieharder,libgcrypt20 and clean up
RUN apt-get update && \
	apt-get install -y --no-install-recommends \
	  dieharder less libgcrypt20 nano && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/* /var/tmp/* /tmp/*

# ------------------------------------------------------------------------------
# Install CHAOSgen binaries to /root
COPY --from=build /code/*.exe /code/test*.sh /root/chaos/

# ------------------------------------------------------------------------------
# Define default command
CMD ["/bin/bash"]
