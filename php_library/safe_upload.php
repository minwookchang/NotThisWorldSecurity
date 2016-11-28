<?php 
/*
파일명 중복시 파일명 뒤에 (n) 추가
file : $_FILES 객체
path : 저장 경로
max_size : 파일 최대 크기

성공시 암호화하지 않은 파일 경로 반환
오류시 오류 메시지 반환
*/

function safe_upload($file, $path, $max_size = 2147483647){
  $hash_opt = ['salt' => 'ThisIsNotRealWorldSalt'];
  $iterator = strtoupper(substr(PHP_OS, 0, 3)) === 'WIN'?'\\':'/';

  //path validation
  if(!is_dir($path)) return "존재하지 않는 경로";
  
  //경로 끝에 /가 없으면 추가해줌
  if($path[strlen($path)-1] !== $iterator) $path.=$iterator;

  if($max_size < $file['size']) return "업로드 파일이 지정된 파일크기보다 큼";

  if(($file['error'] > 0) || ($file['size'] <= 0)) return "오류";

  // HTTP post로 전송된 것인지 체크합니다.
  if(!is_uploaded_file($file['tmp_name'])) return "HTTP로 전송된 파일이 아님";

  //확장자 체크
  $file_name = basename($file['name']);
  if(preg_match('/\.(php|html)$/i', $file_name)) return "php 또는 html 파일";

  //경로 지정
  $path_parts = pathinfo($file_name);
  $file_name = $path_parts['filename'];
  $extension = $path_parts['extension'];

  $uploadfile = $path . password_hash($file_name, PASSWORD_BCRYPT, $hash_opt) .'.'. $extension;

  //파일 이름 재설정
  while(file_exists($uploadfile)){
    $file_name = $file_name.rand(1,1000);
    $uploadfile =  $path . password_hash($file_name, PASSWORD_BCRYPT, $hash_opt) . '.' . $extension;
  }


  //move_uploaded_file은 임시 저장되어 있는 파일을 ./uploads 디렉토리로 이동합니다.
  if(move_uploaded_file($file['tmp_name'], $uploadfile)) {
    return realpath($path) .$iterator. $file_name. '.'. $extension;
  }
  else {
    return "알 수 없는 오류";
  }
}
?>
