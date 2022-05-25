# 2022학년도 4학년 1학기 종합설계프로젝트
### 응용시스템 - 아두이노 스마트 잠금장치 시스템
---
* **아두이노 회로 (Arduino Circuit)**
  
  - Arduino Pro Mini
  - Breadboard Power Supply module
  - Breadboard
  - LED
  - SG90 servo motor
  - RFID-RC522 module

  ![종설프_아두이노회로](https://user-images.githubusercontent.com/71831926/169036423-84be90b9-4e50-4df8-ad3f-1e3ff1d0c8a7.png)
  
  (서보모터를 이용한 잠금장치는 다음과 같이 작동할 예정)
  
  ![22 잠금장치](https://user-images.githubusercontent.com/71831926/170228935-06abc25c-b851-4289-8544-49681f57ee6d.png)
---
* **아두이노 코드 플로우차트 (Arduino Code Flowchart)**
  
  ![arduino_project_flowchart drawio](https://user-images.githubusercontent.com/71831926/168989776-d89758e9-213f-4cc5-999a-066c558a40bf.png)
  
  *RFID/NFC의 UID 정보*와 *자물쇠(모터)의 상태 정보*는 아두이노에 전원 공급이 끊어지거나 reset이 되어도 사라지거나 초기화되면 안되기 때문에 **EEPROM(Electrically Erasable Programmable Read-Only Memory)** 이라는 **비휘발성 메모리** 에 저장하여 유동적으로 관리하는 방향으로 코드를 작성했다. (EEPROM은 eeprom_clear라는 예제 파일로 clear 가능하다.)
  
  기본적으로 RFID/NFC module에 등록된 카드를 태그하면 초록색 LED가 켜지면서 자물쇠의 상태가 변하고 (Lock ↔ Unlock), 등록되지 않은 카드를 태그하면 빨간색 LED가 켜지면서 Access가 거부된다.

  새로운 카드를 등록하고 싶다면 아두이노의 빨간색 reset 버튼을 누르거나 전원을 껐다가 켜서 아두이노를 재시작하고 초록색과 빨간색 LED가 둘 다 켜져있을 때 (2초(수정가능) 내에) 카드를 리더기에 태그하여 인식시키면 등록이 된다.
  
---
* **아두이노 회로 및 코드 실행 영상 (Arduino Performance Video)**

  https://user-images.githubusercontent.com/71831926/169045322-b0dd8a1c-8498-443c-9d6a-e31530022a45.mp4
