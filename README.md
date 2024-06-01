
============================================================

목차 
---------------------
1. 어플리케이션 소개
2. 작동 방법
3. 기능
4. 소감문
   


---------------------
## 어플리케이션 소개 
# http://main.projectgraphic.net./   


--------
블럭깨기 게임을 변형하여 여러가지 기능을 추가하였습니다
-------
##  어플리케이션 작동방법입니다.

git clone을 통해 레포지토리를 복사한후에 디버깅하시면됩니다
------------
## 어플리케이션 기능

시작화면
<img width="596" alt="화면 캡처 2024-06-01 212328" src="https://github.com/sasileunnadojalmorem/game/assets/79616817/d4d3793f-33d8-433d-8674-724717f9065f">

난이도설정화면
<img width="580" alt="화면 캡처 2024-06-01 212336" src="https://github.com/sasileunnadojalmorem/game/assets/79616817/e053fdbf-27c0-4e32-96c8-5048b00caa0b">


블럭의 종류
<img width="592" alt="화면 캡처 2024-06-01 212347" src="https://github.com/sasileunnadojalmorem/game/assets/79616817/c5bdda03-7498-40c0-b66f-c5e5552bf4f0">

게임 오버화면을 제작 하였습니다
<img width="609" alt="화면 캡처 2024-06-01 212426" src="https://github.com/sasileunnadojalmorem/game/assets/79616817/66d0221e-b485-471b-b29c-206e978b8ca9">

---------------------------


## 소감문
* 백경민
* * *

이번 과제를 통하여 SDL2를 활용하여 게임을 개발을 해봄으로서 SDL 라이브러리를 사용하여 그래픽, 입력 처리, 텍스트 렌더링 등의 기초적인 게임 개발 요소를 구현할 수 있는 능력을 키울 수 있었습니다. SDL은 조금만 기능을 추가하여도 라이브러리가 필요해 검색하여 다운받아 파일을 추가하는 번거러움이 있는것 같습니다. 

또한 이 프로젝트를 팀으로 진행하면서 협업의 중요성과 효과적인 커뮤니케이션의 필요성을 깊이 체감할 수 있었습니다. 처음 다른 사람의 코드를 보고 수정 해볼려고 할때 하나하나 찾아보며 이해하는 것이 어려웠습니다.  결과적으로, 팀플로 진행한 이번 프로젝트를 통해 협업의 즐거움과 협력이 프로젝트의 성공에 얼마나 중요한지를 깨닫게 되었습니다. 앞으로도 이러한 협업 경헙을 바탕으로 더 나은 팀 플레이어로 성장할 수 있도록 하겠습니다.
* * *
* 김경오
* * *
이번 프로젝트를 통해, 게임 개발의 기본적인 과정을 이해하고, 실습을 통해 많은 것을 배울 수 있었습니다. 향후에는 이번 프로젝트에서 배운 내용을 바탕으로 더 복잡하고 다양한 기능을 가진 게임을 개발해보고 싶습니다. 또한, 게임 개발에서 중요한 점은 단순히 코드 작성이 아니라, 사용자 경험을 고려한 설계와 디버깅, 최적화 등의 과정임을 알 게 되었습니다.

* * *
 
* 김재형
 * * *

이번 팀 프로젝트에서 저는 게임의 시작화면을 만드는 작업을 담당했고 
시작화면을 만드는 작업을 하면서 SDL에 문자를 화면에 출력하기 위해서
SDL-TTF라는 별도의 라이브러리가 필요한 것을 알게되어 
설치후 프로젝트에 추가한 다음 시작화면에서 

게임의 이름과 게임시작과 난이도 조정을 고르는 선택지를 만들었습니다
한가지 아쉬운건 한글로 출력하면 문자가 깨져서 부득이하게
영어를 사용하게 된 부분이 아쉬웠습니다 이렇게 선택지 부분을 구현한 후

게임의 시작 배경 이미지를 추가하기 위해서는 SDL-TTF처럼 SDL_image를
또 별도로 설치하고 프로젝트에 추가해야 했는데요 이런 과정이 좀 번거롭게
느껴지긴 했습니다만 이번 팀 과제를 하면서 SDL의 문자와 이미지를 작업하는 
부분을 습득할 수 있게 되어 좋았습니다. 

* * *

* 신재민 

* * *

이번 과제에서 SDL을 활용한 게임 개발을 팀 프로젝트로 진행해보았습니다. 제가 맡은 부분은 게임의 난이도 조절이었는데, 시작 화면을 띄우는 코드를 참고하여 난이도를 변경할 수 있도록 구현하였습니다. 선택지에 따라 전역변수인 levelstate를 조정하여 게임 실행 시 변화가 생기도록 설계하였습니다. 이를 통해 플레이어가 게임을 시작하기 전에 난이도를 선택할 수 있게 하고, 그에 따라 게임 플레이에 영향을 주는 요소를 구현했습니다.

SDL을 사용하면서 몇 가지 기능을 추가하기 위해 별도의 파일을 다운로드해야 하는 불편함이 있었습니다. 그러나 팀원들과의 상의를 통해 이러한 문제를 해결하고 필요한 기능을 구현하는 데에는 상당한 도움을 받았습니다. 특히, 팀원들과의 협력을 통해 어려움을 극복하고 함께 문제를 해결하는 과정에서 많은 것을 배우게 되었습니다. 이 경험을 통해 SDL을 사용하여 게임을 개발하는 방법에 대해 더 많은 이해를 얻을 수 있었고, 앞으로의 프로젝트에서도 이를 활용하여 더 나은 결과물을 만들어 나갈 수 있을 것으로 기대됩니다.

* * *

* 전재민

* * *

블럭깨기 게임을 제작하며 PM 역할을 맡아 팀원들에게 역할을 배정하고 도와주는 것은 힘든 일이었지만, 그 과정에서 팀원들의 능력을 발휘하고 게임이 완성되는 과정을 지켜보는 것은 보람찬 경험이었습니다. 함께 노력하여 어려움을 극복하고 최종적으로 완성된 게임을 보는 순간, 우리의 협업과 노력이 빛을 발하며 더 큰 성취감을 느낄 수 있었습니다.
팀원들의 문제점을 함께 해결하는 과정은 힘들기도 하지만 동시에 매우 즐거웠습니다. 각자의 역량과 관점을 살려 문제를 해결하고, 서로의 아이디어를 공유하며 새로운 해결책을 찾아나가는 것은 창의적이고 유익한 경험이었습니다. 함께 노력하고 서로를 이해하며 소통하는 과정에서 우리 팀은 더욱 탄탄한 유대감과 신뢰를 쌓을 수 있었습니다. 이러한 협업은 팀원들 간의 상호 작용을 향상시키고 문제 해결 능력을 향상시켰으며, 최종적으로 게임 제작 과정을 보다 유익하고 즐겁게 만들어주었습니다. 함께 어려움을 극복하고 성공을 경험함으로써 우리는 더 큰 동기부여와 자신감을 얻을 수 있었습니다.
* * *


