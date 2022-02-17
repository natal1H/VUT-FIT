# IIS project - Canteen

## Authors

- Natália Holková (xholko02)
- Samuel Krempaský (xkremp01)
- Matej Novák (xnovak2f)

## Prerequisites

- Docker

or 

- Python 3+
- Django 2+

## Installation

### Using docker

1. Move to directory with Dockerfile: `cd app/`
2. Build docker image (may require `sudo`): `docker build -t canteen-docker .`
3. Run docker container (may require `sudo`): `docker run -it -p 8888:8888 canteen-docker`
4. Website should be now available at http://localhost:8888/

### Not using docker

1. Move to directory with manage.py: `cd app/`
2. Load initial data to database: `python3 manage.py migrate`
3. Collect static files: `python3 manage.py collectstatic --noinput`
4. Run the server: `python3 manage.py runserver`
5. Website should be now available at http://127.0.0.1:8000/
