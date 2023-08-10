import transformers
from transformers import BertTokenizer, DataCollatorWithPadding
from transformers import AutoModelForSequenceClassification
import urllib.parse
tokenizer = BertTokenizer.from_pretrained('huolongguo10/check_sec_tiny')
model = AutoModelForSequenceClassification.from_pretrained('huolongguo10/check_sec_tiny', num_labels=2)
import torch
f=open('/home/huolongguo10/debug','a+')
def _check(text):
    inputs = tokenizer(text, return_tensors="pt")
    with torch.no_grad():
        logits = model(**inputs).logits
    predicted_class_id = logits.argmax().item()
    print(f'{logits.argmax().item()}:{text}')
    f.write(f'{logits.argmax().item()}:{text}\n')
    f.flush()
    return predicted_class_id # 0=secure

def check(text):
    print(text)
    _text=text.split(' ')[0].split('&')   
    for value in _text:
        if _check(urllib.parse.unquote(value))==1:
            return 1
    return 0

print(check('1&2'))