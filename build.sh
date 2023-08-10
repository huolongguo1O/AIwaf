python -m build
git clone https://github.com/nginx/nginx
cd nginx
make clean
sudo /usr/local/nginx/sbin/nginx -s stop 
./auto/configure --add-module=../
make&&sudo make install 
sudo /usr/local/nginx/sbin/nginx -c /usr/local/nginx/conf/nginx.conf
cat /usr/local/nginx/logs/error.log|tail -n 10 
