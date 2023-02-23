# Base image
FROM node:18.14.2-alpine as build

# Set working directory
WORKDIR /app

# Copy package.json and package-lock.json to the working directory
COPY . .

# Install dependencies
RUN npm install

# Build the app
RUN npm run build

# Set the base image
FROM nginx:stable-alpine

# Copy the build files to the nginx web server directory
COPY --from=build /app/build /usr/share/nginx/html

# Copy the nginx config file to the nginx directory
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Expose port 84
EXPOSE 84

# Start nginx
CMD ["nginx", "-g", "daemon off;"]
