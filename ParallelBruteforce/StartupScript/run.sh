#!/bin/bash
#
#mpirun -host 127.0.0.1 -np 2 parallelbruteforce : -np 1 --hostfile mpi_hosts --path /tmp --tmpdir /tmp --prefix /usr parallelbruteforce


function printUsage {
echo -e "usage: bash run.sh <scp_host_file> <hash_list_file>\n\n"
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
echo -e "			    \n\n"
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
}			    
			      

executionPath="/tmp"
binaryName="parallelbruteforce"
prefix="/usr"
tempDir="/tmp"
mpiHostFileName="mpi_hostfile"
numCores=grep -c ^processor /proc/cpuinfo

numProcessors=0;
localExecution=0;
array[0]="0"
unset array[0]

nolocalCounter=0;



# parameter presence checks
if [ $# -eq 0 ]
  then
    printUsage
    echo -e "Error cause:   no parameters\n\n"
    exit -1
fi

if ! [ $# -eq 2 ]
  then
    printUsage
    echo -e "Error cause:   wrong number of parameters (! = 2)\n\n"
    exit -1
fi

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

if ! [ -e $1 ] 
  then
    printUsage
    echo -e "Error cause:   scp_host_file does not exist\n\n"
    exit -1
fi

if ! [ -e $2 ]
  then
    printUsage
    echo -e "Error cause:   hash_list_file does not exist\n\n"
    exit -1
fi

if ! [ -f $1 ]
  then
    printUsage
    echo -e "Error cause:   scp_host_file is not a regular file\n\n"
    exit -1
fi

if ! [ -f $2 ]
  then
    printUsage
    echo -e "Error cause:   hash_list_file is not a regular file\n\n"
    exit -1
fi


if [ -e $mpiHostFileName ]
  then
    rm $mpiHostFileName
fi

touch $mpiHostFileName


atSymbol="@"
while read line           
do   
case "$line" in \#*) continue ;; 
esac
      array[numProcessors]=$line
      numProcessors=$(($numProcessors+1));
      if [ $line = "localhost" ]
      then
	localExecution=1;
      else
	echo "$line slots=1" >> $mpiHostFileName
      fi
    
done <$1

counter=0
while [  $counter -lt $(($numProcessors)) ]; do
   if ! [ ${array[$(($counter))]} = "localhost" ]
  then
    echo "executing: scp $binaryName ${array[$(($counter))]}:$tempDir/$binaryName"
    scp "$binaryName" "${array[$(($counter))]}:$tempDir/$binaryName"
    echo "executing: scp $mpiHostFileName ${array[$(($counter))]}:$tempDir/$mpiHostFileName"
    scp "$mpiHostFileName" "${array[$(($counter))]}:$tempDir/$mpiHostFileName"
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

localExecutionRunInstruction="--host localhost -np 2 $binaryName $2"

remoteExecutionRunInstruction="--hostfile $mpiHostFileName --path $executionPath --tmpdir $tempDir --prefix $prefix --wdir $executionPath -np $remoteProcNum $binaryName $2"



if [ $localExecution = 1 ]
then
  echo "starting mpi: mpirun $localExecutionRunInstruction : $remoteExecutionRunInstruction"
  mpirun $localExecutionRunInstruction : $remoteExecutionRunInstruction

else
  echo "starting mpi: mpirun $remoteExecutionRunInstruction"
  mpirun $remoteExecutionRunInstruction
fi








