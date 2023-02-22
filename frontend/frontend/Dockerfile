# Base image
FROM node:14 as build

# Set working directory
WORKDIR /

# Copy package.json and package-lock.json to the working directory
COPY package*.json ./

# Install dependencies
RUN npm install

# Copy the entire app directory to the working directory
COPY . .

# Build the app
RUN npm run build

# Set the base image
FROM nginx:1.21.1-alpine

# Copy the build files to the nginx web server directory
COPY --from=build /build /usr/share/nginx/html

# Copy the nginx config file to the nginx directory
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Expose port 80
EXPOSE 80

# Start nginx
CMD ["nginx", "-g", "daemon off;"]
