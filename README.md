# drv-hmc5883l-gy85
This driver was made for the hmc5883l Honeywell sensor, mounted on GY85 flight-controller sensor.

In order to initialize the west versioning for the first time:
```shell
$ mkdir .west
$ touch .west/config
$ west config manifest.path .
$ west config manifest.file west.yml 
```

Then to keep the libraries updated:
```shell
$ west update
```