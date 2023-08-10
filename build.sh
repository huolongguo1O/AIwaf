python -m build
cd dist
pip install check-0.3.5-py3-none-any.whl
cd ..

git clone https://github.com/nginx/nginx
cd nginx
make clean
sudo /usr/local/nginx/sbin/nginx -s stop 
echo input your nginx compile flags and press enter:
./auto/configure --add-module=../ `head -n 1`
make&&sudo make install 
sudo /usr/local/nginx/sbin/nginx -c /usr/local/nginx/conf/nginx.conf

