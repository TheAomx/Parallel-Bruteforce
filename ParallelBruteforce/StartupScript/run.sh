#!/bin/bash
#
#mpirun -host 127.0.0.1 -np 2 parallelbruteforce : -np 1 --hostfile mpi_hosts --path /tmp --tmpdir /tmp --prefix /usr parallelbruteforce


function printUsage {
echo -e "usage: bash run.sh <scp_host_file> <hash_list_file> <mpi_hosts_file>\n\n"
echo  "	scp_host_file:      path to a file which defines each host which will"
echo "			    execute the bruteforce programm."
echo "			    Format example (simular to the mpi host file):"
echo "			    ---------------------------------------------------"
echo "			      # a comment"
echo "			      # next definition works if current user is" 
echo "			      # available on remote machine"
echo "			      192.168.42.23"
echo "			      # next definition uses different kind of user"
echo "			      user@192.168.24.32"
echo "			    ---------------------------------------------------"
echo "			    Note that each host defined in <mpi_hosts_file>"
echo "			    also has to be defined in this file in order to"
echo -e "			    transmit required binaries and resources.\n\n"
echo "	hash_list_file:	    path to a file which defines the list of hashes to"
echo "			    find a password for."
echo "			    One hash or hash type identifier per line."
echo "			    Sample:"
echo "			    ---------------------------------------------------"
echo "			       MD5"
echo "			       {MD5_HASH}"
echo "			       {MD5_HASH}"
echo "			       SHA265"
echo "			       {SHA265_HASH}"
echo "			       {SHA265_HASH}"
echo "			       ..."
echo -e "			    ---------------------------------------------------\n\n"
echo "	mpi_hosts_file:     path to a mpi host file "
echo "			    (mpirun prameter -hostfile)."
echo "			    Sample:"
echo "			    ---------------------------------------------------"
echo "			       www.mympiprocessor.com slots=10 max-slots=1000"
echo "			       localhost slots=1 max-slots=1 min-slots=1"
echo "			       ..."
echo -e "			    ---------------------------------------------------\n\n"

}			    
			      

executionPath="/tmp"
binaryName="parallelbruteforce"
prefix="/usr"
tempDir="/tmp"
localExecutionRunInstruction="--host localhost -np 2 $binaryName"

numProcessors=0;
localExecution=0;
array[0]="0"
unset array[0]

nolocalCounter=0;



# parameter presence checks
# echo $# 

if [ $# -eq 0 ]
  then
    printUsage
    echo -e "Error cause:   no parameter specified\n\n"
    exit -1
fi

if ! [ $# -eq 3 ]
  then
    printUsage
    echo -e "Error cause:   wrong number of parameters specified\n\n"
    exit -1
fi

# parameter checks (checks if parameters are empty strings)
if [ -z "$1" ]
  then
    printUsage
echo "Error cause:   scp_host_file parameter not present\n\n"
    exit -1
fi

if [ -z "$2" ]
  then
    printUsage
    echo -e "Error cause:   scp_host_file parameter not present\n\n"
    exit -1
fi

if [ -z "$3" ]
  then
    printUsage
    echo -e "Error cause:   scp_host_file parameter not present\n\n"
    exit -1
fi

# parameter checks (checks if required files are present and regular files)


if ! [ -e $1 ] 
  then
    printUsage
    echo -e "Error cause:   scp_host_file not present\n\n"
    exit -1
fi


if ! [ -e $2 ]
  then
    printUsage
    echo -e "Error cause:   hash_list_file not present\n\n"
    exit -1
fi

if ! [ -e $3 ]
  then
    printUsage
    echo -e "Error cause:   mpi_host_file not present\n\n"
    exit -1
fi


if ! [ -f $1 ]
  then
    printUsage
    echo -e "Error cause:   scp_host_file not a regular file\n\n"
    exit -1
fi

if ! [ -f $2 ]
  then
    printUsage
    echo -e "Error cause:   hash_list_file not a regular file\n\n"
    exit -1
fi
if ! [ -f $3 ]
  then
    printUsage
    echo -e "Error cause:   mpi_host_file not a regular file\n\n"
    exit -1
fi


# end of function checkParameter






while read line           
do   
case "$line" in \#*) continue ;; 
esac
    echo -e "$line"
    
      array[numProcessors]=$line
      numProcessors=$(($numProcessors+1));
      if [ $line = "localhost" ]
      then
	localExecution=1; 
      fi
    
done <$1

echo -e "number of clients = $numProcessors"
echo "List of hosts: ${array[*]}"

if [ localExecution = 1 ]
then
  echo "Local execution enabled"
fi







counter=0
while [  $counter -lt $(($numProcessors)) ]; do
   if [ ${array[$(($counter))]} = "localhost" ]
  then
    echo "skip copying to localhost"    
  else
    echo "executing: scp $binaryName ${array[$(($counter))]}:$tempDir/$binaryName"
    scp "$binaryName" "${array[$(($counter))]}:$tempDir/$binaryName"
    echo "executing: scp $3 ${array[$(($counter))]}:$tempDir/$3"
    scp "$3" "${array[$(($counter))]}:$tempDir/$3"
    echo "executing: scp $2 ${array[$(($counter))]}:$tempDir/$2"
    scp "$2" "${array[$(($counter))]}:$tempDir/$2"
  fi
  let counter=counter+1 
done




remoteProcNum=$numProcessors
if [ $localExecution = 1 ]
then
remoteProcNum=$(($remoteProcNum-1));
fi

remoteExecutionRunInstruction="--hostfile $3 --path $executionPath --tmpdir $tempDir --prefix $prefix --wdir $executionPath -np $remoteProcNum $binaryName"

if [ $localExecution = 1 ]
then
  echo "starting mpi: mpirun $localExecutionRunInstruction : $remoteExecutionRunInstruction"
  mpirun $localExecutionRunInstruction : $remoteExecutionRunInstruction

else
  echo "starting mpi: mpirun $remoteExecutionRunInstruction"
  mpirun $remoteExecutionRunInstruction
fi







