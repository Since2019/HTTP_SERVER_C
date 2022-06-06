require 'net/http'

$i = 0
$num = 150
 
Net::HTTP::HTTPVersion = '1.1'
while $i < $num  do
    Net::HTTP.get('127.0.0.1', '/index2.html')
   $i +=1
end
