# ------------------------------------------------------------------------------
# Install build tools and compile CHAOSgen
FROM ubuntu:focal AS build
ADD src /code
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
	  apcalc dieharder less libgcrypt20 nano && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/* /var/tmp/* /tmp/*

# ------------------------------------------------------------------------------
# Update .bashrc
RUN echo >>/root/.bashrc && \
	echo "cd /root/chaos" >>/root/.bashrc

# ------------------------------------------------------------------------------
# Install CHAOSgen binaries to /usr/bin/ and /root/chaos/
COPY --from=build /code/*.exe /usr/bin/
COPY --from=build /code/*.exe /code/test*.sh /root/chaos/

# ------------------------------------------------------------------------------
# Define default command
CMD ["/bin/bash"]
