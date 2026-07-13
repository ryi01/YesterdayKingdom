<img width="893" height="503" alt="image" src="https://github.com/user-attachments/assets/3c1a1a4e-70bb-457a-83af-037ce1c6e52c" /># 어제는 왕국 멀쩡했음

절차적으로 생성되는 던전을 탐험하며 전투와 성장을 반복하는  
**Unreal Engine 5 기반 3D 액션 RPG 팀 프로젝트**입니다.

퀘스트 수락부터 던전 생성, 일반 적과 엘리트 전투, 보스 처치까지 이어지는 전체 플레이 흐름을 구현했습니다.  
플레이어 전투, 적 AI, 절차적 던전, 인벤토리, 장비, 상점, 스킬트리, Save / Load 시스템을 하나의 구조로 연결하는 데 집중했습니다.

---

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트명 | 어제는 왕국 멀쩡했음 |
| 장르 | 3D 액션 RPG |
| 개발 인원 | 3인 |
| 개발 기간 | 2026.05.19 ~ 2026.06.18 |
| 개발 환경 | Unreal Engine 5 / C++ |
| 담당 역할 | 팀장 / 공통 시스템 구조 / 보스 FSM / 데이터 연동 |

---

## 시연 영상

[![시연 영상](<img width="893" height="503" alt="image" src="https://github.com/user-attachments/assets/b476c2b4-2699-4f1c-bc6d-483c76edb495" />
)](https://drive.google.com/file/d/1jk0sM3enjO3qKLUQpDXSVBdlS0Zr8ad5/view?usp=drive_link)

---

## 핵심 구현

- 약공격 / 강공격 / 차지 공격 기반 근접 전투
- DataTable 기반 공격 데이터 관리
- StateTree 기반 일반 적 AI
- FSM 기반 엘리트 및 보스 AI
- Delaunay Triangulation과 MST 기반 절차적 던전 생성
- 인벤토리 / 장비 / 상점 / 스킬트리 시스템
- SQLite 기반 Save / Load 시스템
- Component와 Delegate 기반 데이터·UI 연동

---

# 게임 흐름

<img width="2498" height="891" alt="image" src="https://github.com/user-attachments/assets/61a4f65e-0c20-4933-bdc4-a1926732e46f" />

```text
퀘스트 수락
→ 던전 생성 및 입장
→ 일반 적 전투와 탐험
→ 엘리트 처치
→ 보스 방 진입
→ 보스 전투
→ 던전 클리어
```

플레이어는 전투와 탐험 과정에서 획득한 골드와 아이템을 활용해 장비를 교체하고,  
상점과 스킬트리를 통해 전투 능력을 강화할 수 있습니다.

---

# 시스템 구조

## 1. 캐릭터 상속 구조

> 아래 이미지를 캐릭터 상속 구조 이미지로 교체해 주세요.

<img width="1518" height="716" alt="image" src="https://github.com/user-attachments/assets/a7895123-db89-4922-85e4-7cfc3894ec4c" />

```text
BaseCharacter
├── PlayerCharacter
└── EnemyBase
    ├── NormalEnemy
    ├── EliteEnemy
    └── Boss
```

공통 피격, 공격, 사망 처리는 `BaseCharacter`에서 관리하고,  
플레이어와 적의 전용 기능은 하위 클래스에서 확장하도록 구성했습니다.

### 구조적 특징

- 공통 피격 / 공격 / 사망 처리 재사용
- Player와 Enemy의 전용 기능 분리
- 적 유형에 따른 AI 및 공격 패턴 확장
- 중복 로직 감소 및 유지보수성 향상

---

## 2. Component 기반 기능 분리

캐릭터 클래스에 모든 기능을 직접 구현하지 않고, 기능별 책임을 Component 단위로 분리했습니다.

### Player Component

| Component | 역할 |
| --- | --- |
| `StatComponent` | HP, 스태미나, 마나 및 능력치 관리 |
| `CombatComponent` | 공격 입력과 전투 처리 |
| `InventoryComponent` | 아이템 보관 및 사용 |
| `EquipmentComponent` | 장비 장착과 능력치 적용 |
| `GoldComponent` | 골드 획득 및 소비 |
| `QuestComponent` | 퀘스트 진행 상태 관리 |

### Enemy Component

| Component | 역할 |
| --- | --- |
| `StatComponent` | 적 능력치 관리 |
| `CombatComponent` | 적 공격 및 피해 처리 |
| `FSMControllerComponent` | 상태 및 AI 전투 흐름 관리 |

---

## 3. 데이터 처리 구조

```text
DataTable
→ Component
→ Delegate
→ Widget
```

- `DataTable` : 아이템, 공격, 능력치 데이터 관리
- `Component` : 데이터 조회 및 게임 로직 처리
- `Delegate` : 데이터 변경 이벤트 전달
- `Widget` : 변경된 상태를 화면에 표시

Widget이 데이터를 직접 수정하지 않고 Component의 상태 변경 결과를 전달받도록 구성해  
데이터 처리와 UI 표시 역할을 분리했습니다.

---

# 주요 시스템

## 1. 플레이어 전투 시스템

<img width="593" height="285" alt="image" src="https://github.com/user-attachments/assets/c8e95298-2743-46d5-9971-917a3a5528ac" />

### 조작

| 입력 | 기능 |
| --- | --- |
| `WASD` | 이동 |
| `Shift` | 대시 |
| `마우스 좌클릭` | 약공격 |
| `마우스 우클릭` | 강공격 |
| `Q` | 차지 공격 |
| `F` | 가드 |
| `E` | 상호작용 |
| `Tab` | 인벤토리 |

### 구현 내용

- 약공격 / 강공격 / 차지 공격
- 공격 입력과 이동 입력 분리
- 공격 Montage 재생
- Sphere Sweep 기반 공격 판정
- Anim Notify 기반 판정 타이밍 제어
- 공격별 자원 소모
- 피격 리액션 및 데미지 처리
- 스킬 해금 여부에 따른 전투 기능 활성화

---

## 2. 데이터 기반 공격 구조

공격 종류와 공격 정보를 코드에 직접 작성하지 않고 `AttackDataTable`에서 관리했습니다.

```text
AttackType
→ RowName
→ AttackDataTable 조회
→ 공격 데이터 적용
```

### 관리 데이터

- 공격 타입
- 공격 Montage
- 공격력
- 소모 자원
- 차지 시간
- 차지 데미지 배율
- 이동 및 점프 수치
- 공격 판정 거리
- 공격 판정 범위

공격 로직과 수치를 분리해 코드 수정 없이 DataTable에서 밸런스를 조정할 수 있도록 구성했습니다.

---

## 3. 공격 실행 흐름

```text
입력 감지
→ 공격 데이터 조회
→ 공격 Montage 실행
→ 공격 범위 판정
→ 데미지 및 피격 처리
```

1. 입력에 따라 공격 종류 결정
2. DataTable에서 공격력, 자원 소모량, Montage 조회
3. 공격 Montage 실행
4. Anim Notify 시점에 Sphere Sweep 수행
5. 피격 대상에게 데미지와 리액션 적용

애니메이션의 실제 타격 시점에 공격 판정을 실행해  
시각적 타격과 실제 판정 사이의 차이를 줄였습니다.

---

## 4. Enemy AI 구조

> 아래 이미지를 적 AI 구조 이미지로 교체해 주세요.

<img width="902" height="188" alt="image" src="https://github.com/user-attachments/assets/3fdb5794-eb31-4ecd-b9e1-80ec22563de7" />

```text
EnemyBase
├── NormalEnemy : StateTree
├── EliteEnemy  : FSM
└── Boss        : FSM
```

적 유형의 행동 복잡도에 따라 일반 적은 `StateTree`,  
엘리트와 보스는 `FSM`을 적용했습니다.

모든 적은 EQS를 활용해 순찰 지점, 측면 이동 위치, 복귀 위치를 탐색합니다.

---

## 5. 일반 적 StateTree

> 아래 이미지를 StateTree 에디터 이미지로 교체해 주세요.

<img width="240" height="385" alt="image" src="https://github.com/user-attachments/assets/be91ba3e-1a07-43c5-a283-53be03270e4f" />

### 주요 상태

- Idle
- Patrol
- Detect
- Chase
- Attack
- Flanking
- Return
- Damage
- Death

### 구현 내용

- 감지 거리에 따른 추적 전환
- 공격 거리에 따른 공격 전환
- EQS 기반 순찰 지점 탐색
- 측면 이동 위치 탐색
- 플레이어가 범위를 벗어나면 원래 위치로 복귀
- 피격 및 사망 상태 분리
- Task와 Transition 기반 상태 제어

---

## 6. 엘리트 AI

엘리트 적은 공통 FSM 구조를 기반으로 일반 적보다 확장된 전투 흐름을 구현했습니다.

```text
Idle
→ Patrol
→ Chase
→ Attack
→ Cooldown
→ Return
```

### 구현 내용

- 명시적인 상태 전환
- 공격 후 Cooldown 처리
- 전투 범위 이탈 시 Return
- Damage / Death 상태 분리
- 엘리트 전용 부활 구조
- 상태별 로직 분리

---

## 7. 보스 FSM 및 전투 패턴

> 아래 이미지를 보스 FSM 또는 보스 전투 이미지로 교체해 주세요.

<img width="641" height="137" alt="image" src="https://github.com/user-attachments/assets/5ea2c1e4-8b0d-4b6f-ae1b-cacea10af847" />

보스는 거리, 쿨다운, 현재 페이즈와 전투 상황을 확인한 뒤  
적절한 공격 패턴을 선택하는 FSM 기반 AI로 구현했습니다.

### 주요 상태

- Idle
- Chase
- PatternSelect
- Attack
- AttackAction
- Cooldown
- BackStep
- Flanking
- PhaseChange
- Groggy
- Damage
- Death

### 공격 패턴

- 약공격 패턴
- 강공격 패턴
- 회전 공격
- 내려찍기
- 점프 공격
- 페이즈 전환 공격

### 구현 내용

- 거리 기반 공격 패턴 선택
- 공격별 Cooldown 관리
- 후방 이동 및 측면 이동
- 체력 조건에 따른 페이즈 변경
- 페이즈별 패턴 확장
- 공격 시작과 종료 상태 관리
- 공격 애니메이션과 판정 연동
- 보스 전용 BGM 및 효과 연동

`PatternSelect`와 `AttackAction`을 분리해  
패턴 선택 조건과 실제 공격 실행 로직을 독립적으로 관리했습니다.

---

## 8. 절차적 던전 생성

> 아래 이미지를 던전 생성 구조 이미지로 교체해 주세요.

<img width="3120" height="1111" alt="image" src="https://github.com/user-attachments/assets/98ad7b56-34a5-4388-b93e-2197597c4b9f" />

### 생성 흐름

```text
Room 후보 생성
→ 방 간 겹침 제거
→ Delaunay Triangulation
→ MST 생성
→ 일부 간선 추가
→ BFS로 메인 경로 탐색
→ Room Type 지정
→ 던전 생성
```

### 구현 내용

- 랜덤 방 배치
- Delaunay Triangulation 기반 연결 후보 생성
- MST 기반 필수 연결 구조 생성
- 일부 간선 추가를 통한 우회 경로 생성
- BFS 기반 시작 방과 보스 방의 메인 경로 탐색
- Start / Battle / Elite / Store / Boss Room 구성
- 방 사이 통로와 벽 배치
- 보스 방 진입 조건 관리

MST를 통해 모든 방의 연결을 보장하고, 일부 간선을 추가해  
단조로운 트리 구조 대신 우회 경로와 탐험 선택지를 만들었습니다.

---

## 9. 특수 방 배치

| 방 종류 | 역할 |
| --- | --- |
| Start Room | 던전 시작 지점 |
| Battle Room | 일반 적 전투 및 보상 획득 |
| Store Room | 아이템 구매 및 전투 재정비 |
| Elite Room | 엘리트 전투 및 보스 진입 조건 달성 |
| Boss Room | 최종 보스 전투 및 던전 클리어 |

```text
Start Room
→ Battle Room
→ Store Room
→ Elite Room
→ Boss Room
```

보스 전투 전에 상점에서 재정비하고 엘리트 전투를 반드시 거치도록  
메인 경로와 인접 관계를 기준으로 특수 방을 배치했습니다.

---

## 10. 아이템 및 인벤토리 시스템

> 아래 이미지를 인벤토리 이미지로 교체해 주세요.

<img width="325" height="182" alt="image" src="https://github.com/user-attachments/assets/6ef723b0-d8e1-4603-974e-63721350aa11" />

### 구현 내용

- DataTable 기반 아이템 데이터 관리
- 아이템 획득 및 인벤토리 추가
- 소비 아이템 사용
- 장비 아이템 장착
- 아이템 타입별 필터
- 아이템 선택 및 상세 정보 표시
- 상점 구매 / 판매 연동
- SQLite 저장 및 로드 연동

---

## 11. 장비 시스템

### 장비 슬롯

- Head
- Body
- Legs

### 구현 내용

- 장비 슬롯별 장착
- 기존 장비 교체
- 장비 능력치 적용 및 해제
- 인벤토리와 장비 UI 연동
- 장착 상태 저장 및 로드

---

## 12. 상점 시스템

> 아래 이미지를 상점 이미지로 교체해 주세요.

<img width="324" height="184" alt="image" src="https://github.com/user-attachments/assets/df29d0c2-ff05-4b46-b220-6ccc8ff19ab1" />

### 구현 내용

- 상점 NPC 상호작용
- 상점 UI 열기 / 닫기
- 아이템 구매
- 아이템 판매
- 골드 증감 처리
- 인벤토리 연동
- 구매 가능 여부 확인
- 판매 후 인벤토리 상태 갱신

```text
상점 아이템 선택
→ 골드 확인
→ 골드 차감
→ 인벤토리 추가
→ UI 갱신
```

---

## 13. 스킬트리 시스템

> 아래 이미지를 스킬트리 이미지로 교체해 주세요.

<img width="446" height="250" alt="image" src="https://github.com/user-attachments/assets/8fc5b0a9-259a-4dbe-91f5-c4f9d341a8a8" />

### 스킬 유형

- Attack
- Defense
- Utility
- Buff

### 적용 효과

- 공격력 증가
- 방어력 증가
- 최대 HP 증가
- 최대 MP 증가
- 최대 스태미나 증가
- 이동 속도 증가
- 차지 공격 해금
- 패링 해금
- 전투 버프 해금

### 구현 내용

- 선행 스킬 조건 확인
- 골드 보유량 확인
- 스킬 해금 상태 저장
- 해금 즉시 플레이어 능력치 적용
- 해금 상태에 따른 노드 UI 갱신

---

## 14. Save / Load 시스템

> 아래 이미지를 ERD 이미지로 교체해 주세요.

<img width="307" height="163" alt="image" src="https://github.com/user-attachments/assets/6615e711-97db-42dc-9d5c-a41ec8a54de0" />


```text
PlayerData
├── Inventory
├── Equipment
└── SkillTree

Inventory / Equipment
└── ItemMaster
```

### 테이블 역할

| Table | 역할 |
| --- | --- |
| `PlayerData` | 플레이어 기본 정보와 능력치 저장 |
| `Inventory` | 보유 아이템과 수량 저장 |
| `Equipment` | 장착 부위와 장착 아이템 저장 |
| `SkillTree` | 해금한 스킬 정보 저장 |
| `ItemMaster` | 아이템 이름, 타입, 가격, 설명 관리 |

### 구현 내용

- PlayerId 기반 저장 데이터 연결
- 외래키를 활용한 데이터 관계 구성
- Prepared Statement 기반 쿼리 처리
- DataTable 아이템 정보를 ItemMaster에 동기화
- Inventory와 ItemMaster JOIN 조회
- 플레이어 데이터 Save / Load
- 인벤토리, 장비, 스킬트리 상태 복원

---

# 문제 해결 및 개선

## 1. 절차적 던전의 특수 방 배치 안정화

### 문제

- Store Room과 Elite Room이 무작위 위치에 배치됨
- 특수 방이 같은 위치에 중복 지정됨
- Store Room이 생성되지 않는 경우 발생
- 보스 전투 전 재정비 및 엘리트 전투 흐름을 보장하기 어려움

### 해결

```text
BFS 메인 경로 탐색
→ Boss 직전 방을 Elite Room으로 지정
→ 메인 경로 인접 Side Room을 Store Room으로 지정
→ 후보가 없으면 대체 방 탐색
→ 이미 지정된 특수 방 제외
```

### 결과

- Store Room 생성 위치 보장
- 보스 직전 Elite 전투 보장
- 특수 방 중복 문제 해결
- 던전의 필수 진행 순서 안정화

---

## 2. DB 구조 개선 - ItemMaster JOIN

### 문제

초기 데이터베이스에는 저장과 로드에 필요한 정보만 존재해  
DB만으로 아이템 이름, 타입, 가격과 같은 상세 정보를 확인하기 어려웠습니다.

### 해결

- Inventory에는 `ItemRowName`과 수량만 저장
- DataTable의 아이템 정보를 ItemMaster에 동기화
- Inventory와 ItemMaster를 `ItemRowName`으로 JOIN
- 보유 상태와 아이템 상세 정보 분리

### 결과

- 아이템 상세 정보 JOIN 조회 가능
- 저장 데이터 구조 명확화
- 데이터 중복 감소
- 관계형 데이터베이스 활용도 향상

---

## 3. StateTree 기반 적 AI 구조 설계

### 문제

- StateTree 관련 자료와 구현 예제가 부족함
- C++ Task와 에디터 노드 연결 방식 이해가 어려움
- 상태 전환 기준을 설계하고 검증하기 어려움

### 해결

- 상태별 Task를 C++ 클래스로 분리
- Enter / Tick / Exit 역할 구분
- Transition 조건을 에디터에서 구성
- 디버그 로그로 현재 상태와 전환 흐름 확인
- 순찰, 공격, 측면 이동, 복귀 기능을 Task 단위로 구현

### 결과

- 코드와 에디터의 역할 분리
- 상태 전환 안정화
- Task 단위 확장 구조 확보
- 에디터에서 AI 흐름 확인 가능

---

## 4. UI 구조 및 가독성 개선

### 문제

- 기능 추가 과정에서 레이어와 Widget 구조가 복잡해짐
- 정보 배치와 시선 흐름에 대한 기준 부족
- 기능, 가독성, 디자인을 함께 고려한 경험 부족

### 해결

- 게임 UI 레퍼런스 분석
- 화면별 핵심 정보의 우선순위 설정
- 공통 Widget 요소 분리
- 불필요한 레이어 제거
- 팀원 피드백을 반영한 반복 수정
- Delegate 기반 필요한 UI만 갱신

### 결과

- 주요 정보의 가독성 개선
- Widget과 레이어 관리 복잡도 감소
- 데이터 변경과 화면 갱신 구조 분리
- 기능성과 디자인을 함께 고려한 UI 구성

---

# 역할 분담

| 이름 | 담당 |
| --- | --- |
| 김민경 | 팀장 / 공통 시스템 구조 / 보스 FSM 및 패턴 / DataTable·UI 데이터 연동 |
| 김병모 | 일반 적 AI 및 전투 / 엘리트 적 FSM / 적 애니메이션·전투 연동 |
| 이현 | 플레이어 입력·조작 / 전투 입력 시스템 연동 / UI·UX 설계 및 제작 |

---

# 담당 구현

## 공통 시스템 구조

- BaseCharacter 기반 캐릭터 상속 구조 설계
- Stat / Combat / Inventory / Equipment 등 Component 구조 설계
- DataTable → Component → Widget 데이터 흐름 구성
- Delegate 기반 UI 갱신 구조 연동

## 보스 시스템

- FSM 기반 보스 AI 구조
- PatternSelect 및 AttackAction 상태 분리
- 공격 패턴 선택과 전환
- 거리와 Cooldown 기반 행동 결정
- 페이즈 변경
- 보스 공격, 사운드, 이펙트 연동

## 데이터 및 Save / Load

- DataTable 기반 게임 데이터 관리
- SQLite 데이터베이스 연결
- PlayerData 중심 테이블 관계 설계
- Inventory / Equipment / SkillTree 저장
- ItemMaster 동기화 및 JOIN 조회
- 저장 데이터와 UI 연동

## 절차적 던전 및 게임 진행

- 던전 방 역할과 진행 구조 설계
- 특수 방 배치 안정화
- 보스 방 진입 조건 및 던전 클리어 흐름 연동

---

# 기술 스택
- Unreal Engine 5
- C++
- Blueprint
- UMG
- StateTree
- EQS
- FSM
- DataTable
- SQLite
- Delaunay Triangulation
- Minimum Spanning Tree
- Breadth-First Search
- Git / GitHub

---

# 회고

이 프로젝트를 통해 개별 기능을 구현하는 것뿐만 아니라  
여러 시스템이 하나의 플레이 흐름으로 연결되도록 설계하는 과정의 중요성을 경험했습니다.

플레이어 전투, 적 AI, 절차적 던전, 인벤토리, 장비, 상점과 스킬트리는  
각각 독립된 시스템이지만 실제 게임에서는 데이터와 이벤트를 통해 지속적으로 상호작용합니다.

이에 따라 캐릭터의 공통 기능은 상속 구조로 관리하고,  
독립적인 기능은 Component로 분리했으며,  
게임 데이터는 DataTable과 데이터베이스에서 관리하도록 구성했습니다.

절차적 던전에서는 단순히 무작위 구조를 생성하는 것에 그치지 않고,  
상점과 엘리트 방이 실제 플레이 흐름에 맞는 위치에 배치되도록  
알고리즘과 게임 진행 구조를 함께 고려했습니다.

팀장으로서 각 팀원의 시스템이 연결되는 지점을 확인하고,  
공통 데이터 구조와 진행 흐름을 조율하며  
협업에서는 개별 기능의 완성도뿐 아니라 인터페이스와 책임 범위를  
명확히 정하는 것이 중요하다는 점을 경험했습니다.
