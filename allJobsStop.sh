if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <server_pid>"
    exit 1
fi

while read -r line
do
	IFS=$'\t'
	set $line
    ./jobCommander stop $1
done < <(./jobCommander poll queued)

while read -r line
do
	IFS=$'\t'
	set $line
    ./jobCommander stop $1
done < <(./jobCommander poll running) 