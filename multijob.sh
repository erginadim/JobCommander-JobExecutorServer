if ["$#" -eq 0]; then
    echo "Usage: $0 <file1> <file2> ... <fileN>"
    exit 1
fi

for i; do
while read line
    do
    $line
    echo "----------------------"
    done < $i
done
exit 0    