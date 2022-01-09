#!bash
rm -r ./test_output
mkdir ./test_output

for fullname in test_data/*; do
    filename="${fullname##*/}"
    echo "testing" ${fullname}
    ./nmsl < "$fullname" > "./test_output/${filename}.out"
done
