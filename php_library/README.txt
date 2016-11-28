PHP 또는 HTML 파일의 업로드를 막습니다.
파일명 중복시 파일명 뒤에 임의의 수를 이어붙입니다.
파일의 이름을 해시처리하여 저장합니다.

반환값 :

성공시 해싱 전의 파일 경로
오류시 오류 메시지


인자 :

file : $_FILES 객체
path : 저장 경로
max_size : 파일 최대 크기


사용 예시 :

include_once('./safe_upload.php');

$path = './upload/';
$max_size = 20000; //선택 사항

echo safe_upload($_FILES['posted_file'], $path, $mas_size);
