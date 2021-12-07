all: search cluster

CC = g++ -O2
FLAGS = -std=c++14 -c -g #-Wall

OBJS1 = search_main.o functions.o Handling_input.o bucket.o LSH.o hypercube.o discretef.o
OBJS2 = Cluster_main.o functions.o Handling_input_cluster.o Cluster.o LSH.o bucket.o hypercube.o discretef.o

EXEC1 = search
EXEC2 = cluster

$(EXEC1): $(OBJS1)
	$(CC)  $(OBJS1) Fred-master/src/*.cpp -o $(EXEC1)

$(EXEC2): $(OBJS2) 
	$(CC)  $(OBJS2) Fred-master/src/*.cpp -o $(EXEC2)

Handling_input_cluster.o: kMeans++/Handling_input_cluster.cpp
	$(CC) $(FLAGS) kMeans++/Handling_input_cluster.cpp

discretef.o: curves/discretef.cpp
	$(CC) $(FLAGS) curves/discretef.cpp

Handling_input.o: LSH_Folder/Handling_input.cpp
	$(CC) $(FLAGS) LSH_Folder/Handling_input.cpp

functions.o: functions/functions.cpp
	$(CC) $(FLAGS) functions/functions.cpp

Cluster.o: kMeans++/Cluster.cpp
	$(CC) $(FLAGS)  kMeans++/Cluster.cpp

bucket.o: LSH_Folder/bucket.cpp
	$(CC) $(FLAGS) LSH_Folder/bucket.cpp

LSH.o: LSH_Folder/LSH.cpp
	$(CC) $(FLAGS) LSH_Folder/LSH.cpp

hypercube.o: Hypercube/hypercube.cpp
	$(CC) $(FLAGS) Hypercube/hypercube.cpp

Cluster_main.o: Cluster_main.cpp
	$(CC) $(FLAGS) Cluster_main.cpp

search_main.o: search_main.cpp
	$(CC) $(FLAGS) search_main.cpp


clean:
	rm -f $(EXEC1) $(EXEC2) $(OBJS1) $(OBJS2)
