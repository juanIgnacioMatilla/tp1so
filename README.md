This is a special project for Operating systems.

First the docker image must be pulled

docker pull agodio/itba-so:1.0

then a container must be created in the root folder of the project in order to compile it in the same enviroment.

docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name <NAME> agodio/itba-so:1.0

docker start <NAME>

docker exec -ti <NAME> bash

after that run 

make all 

in order to compile the project

To execute run

./master <PATH>/* | ./vista

or

first 

./master <PATH>/* &

and then 

./vista <SHM_NAME>

the SHM_NAME should be the master's output.

Pedro Curti
Juan Ignacio Matilla
Felipe HIba


