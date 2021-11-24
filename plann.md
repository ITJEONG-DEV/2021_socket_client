## 0. user id 얻어내기
> 2021.11.24 구현 완료: 00_user_id

## 1. message parsing 구현
### A. 0x10 타입에 필요한 정보들이 주어졌을 때, sending_message 만들기

### B. 0x11 타입의 예시가 주어졌을 때, 필요한 정보들을 분리하여 반환하기

### C. data의 size 계산하기
> 2021.11.24 구현 완료: 01_message_parser


## 2. option select & 분기를 구현


## 3. file read/write test


## 4. msg_type 별 분기가 작동하도록 구현한다
### A. 0x10: send message

### B. 0x11: recv message

### C. 0x20: send request (uploading file)
###    0x21: recv responce (msg_end: 0x00 = ok / 0x01 = no)
###    0x22: send file data (msg_end: 0x00 = last one / 0x01 = sending)
###    0x23: recv responce

### D. 0x30: send request (downloading file)
###    0x31: recv responce (msg_end: 0x00 = ok / 0x01 = no)
###    0x32: recv file data (msg_end: 0x00 = last one / 0x01 = sending)
###    0x33: send responce

### E. 0x50: send connection closing message (data: EXIT)
###    0x51: recv responce (msg_end: 0x00 = ok)


## 5. 각 msg_type 별 테스트를 진행한다
### A. 0x10: send message

### B. 0x11: recv message

### C. 0x20: send request (uploading file)
###    0x21: recv responce (msg_end: 0x00 = ok / 0x01 = no)
###    0x22: send file data (msg_end: 0x00 = last one / 0x01 = sending)
###    0x23: recv responce

### D. 0x30: send request (downloading file)
###    0x31: recv responce (msg_end: 0x00 = ok / 0x01 = no)
###    0x32: recv file data (msg_end: 0x00 = last one / 0x01 = sending)
###    0x33: send responce

### E. 0x50: send connection closing message (data: EXIT)
###    0x51: recv responce (msg_end: 0x00 = ok)
