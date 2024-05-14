# Use an official GCC runtime as a parent image
FROM gcc:latest

# Install necessary dependencies including CMake, Boost, and Asio
RUN apt-get update && apt-get install -y \
    cmake \
    libboost-system-dev \
    libboost-filesystem-dev \
    libboost-thread-dev \
    libboost-regex-dev \
    libboost-program-options-dev \
    libasio-dev \
    git

# Set the working directory in the container
WORKDIR /usr/src/app

# Clone the Crow repository
RUN git clone https://github.com/CrowCpp/Crow.git

# Copy the contents of the backend directory into the container
COPY backend/ .

# Create a build directory and run CMake
RUN mkdir build
WORKDIR /usr/src/app/build
RUN cmake ..
RUN make

# Make port 8080 available to the world outside this container
EXPOSE 8080

# Run the chess engine
CMD ["./UltimateChess"]
