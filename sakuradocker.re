= Dockerfileは友達！ベストプラクティス入門の入門編

== 技術書同人誌博覧会に寄せて

技術書同人誌博覧会に参加の皆さん、こんにちは、はじめまして。さくらインターネット株式会社@<fn>{sakura}のDeveloper Advocateをしています前佛雅人（ぜんぶつまさひと：Twitter @zembutsu）と申します。ふだんの業務では、学校支援プロジェクト@<fn>{sakura2}を通して小学校プログラミング教育の支援なり、会社のサービス紹介をしたり、技術的なドキュメントを書いたりしています。

今回、記事を書ける機会を頂き、何を書こうかと考えていました。ここは「"技術書"同人誌博覧会」ですから、テーマとしては「Docker」が妥当ではないかと。Dockerとは、誰でも簡単にコンテナを通して様々なアプリケーションを簡単に構築、移動、実行できるプラットフォーム。docker runと実行するだけで、いろいろなモノが簡単に起動できる。私は好きです。そんなDockerを、手足のような道具として扱うには、Dockerイメージを自動構築するDockerfileの理解が欠かせません。

効率的に素早くDockerイメージを構築するための手ほどき・知見が、Dockerfileベストプラクティスであり、Docker公式ドキュメント@<fn>{sakura3}にまとめられています（日本語版@<fn>{sakura4}）。本記事を通し、Dockerを日常の開発などの業務だけで使うのではなく、趣味なども含めた幅広い画面でDockerを使う機会が増えるのを願っています。

//footnote[sakura][さくらインターネット株式会社 https://www.sakura.ad.jp/]
//footnote[sakura2][さくらのプログラミング教育ポータル https://prog-edu.sakura.ad.jp/]
//footnote[sakura3][Best practices for writing Dockerfiles https://docs.docker.com/develop/develop-images/dockerfile_best-practices/]
//footnote[sakura4][Dockerfileを書くためのベストプラクティス【参考訳】v18.09　https://qiita.com/zembutsu/items/a96b68277d699f79418d]

== Dockerイメージを自動構築するDockerfile

Dockerコンテナとしてプロセスを実行するとき、元になるDockerイメージには、ファイル群やメタ情報（自動実行コマンドを指定する、公開するポート番号を指定する、ラベルを指定する等の情報）を含みます。このDockerイメージとはイメージ・レイヤの積み重ねで構成されています。大部分のDockerイメージは、何らかのLinuxディストリビューションを土台（ベース）として構築されています。そのため通常、この元になるイメージをベース・イメージと呼びます。

何らかのDockerイメージを使ってコンテナを実行しますと、読み書き可能なイメージ・レイヤが作成されます。ここに、様々なアプリケーションやミドルウェアをセットアップしたり、設定ファイルの設置をしたり、様々な変更を加えます。そして、最終的にこのイメージ・レイヤをコミット（docker commit）することにより、新しいDockerイメージが作成できます。

さて、このDockerイメージの作成ですが、毎回手作業でセットアップするのは大変です。Dockerの基本コンセプトは、開発者が簡単に扱えるはず……なのに、Dockerイメージを作成するために煩雑でミスのしやすい手作業が必要になってしまうのは、本末転倒です。

この点、Dockerにはイメージを自動構築するための仕組みがあります。それがDockerfileを使ったイメージの自動構築（ビルド）機能です。Dockerfileは単なるテキストファイルですが、１行１行に書かれた命令がイメージ・レイヤに相当します（ただし、イメージは概念的なものであり、実際のファイルシステム上に、命令ごとに実体及び実装としてレイヤのは存在していません）。

命令を書いたDockerfileを準備したあとは、docker buildコマンドを使ってDockerイメージを自動構築します（例：docker build -t タグ名 .）。Dockerイメージが完成したら、あとは他の既存イメージ同様に、docker runコマンドで実行できるようになります。(@<img>{fig1}参照)

//image[fig1][イメージとDockerfile]

== シンプルなDockerfileの例

以下はUbuntuをベースとしてPythonアプリケーションを実行する、シンプルなDockerfileの例です。少しでもDockerfileを触られたことがある方にとっては、見飽きてしまっているよう典型的なものでしょう。

//emlist{
FROM ubuntu:18.04
COPY . /app
RUN make /app
CMD python /app/app.py
//}

この例は、元になるベース・イメージを「FROM」命令で指定し、「COPY」でファイルをコンテナに移動。そして「RUN」でコマンドを実行し、「CMD」でコンテナ実行時にコンテナ内で実行するプログラムを指定しています。

もう少し細かく見てみましょう。Dockerfileは各行で「命令 引数」の組み合わせで構成されています。１行目「FROM」命令は、土台となるイメージを指定する命令です。ここではuubuntu18:04のイメージを使う指定をしています。２行目の「COPY」命令では、「.」（docker build時のカレント・ディレクトリ）をコンテナ内の「/app」ディレクトリにコピーします。３行目の「RUN」命令は、起動するコンテナ内で「make /app」を実行します。そして、最後の４行目の「CMD」命令は、このイメージでコンテナの実行時、何も引数の指定がなければ「python /app/app.py」を実行します。

== Dockerfileの一般的なガイドライン

Dockerfileを使う上で、効率的かつ素早いイメージを構築するためには、以下のガイドラインがドキュメント中では示されています。

 * エフェメラル（使い捨て型）コンテナを作成すべし
 * ビルド・コンテクストについて理解しよう
 * .dockerignore で除外
 * マルチ・ステージ・ビルドを使う
 * 不要なパッケージをインストールしない
 * アプリケーションの分離について考慮
 * レイヤ数を最小化
 * 複数行にわたる引数の順番
 * 構築キャッシュを活用する

この中でも重要なのは、なぜ、このようなガイドラインが存在するかです。

=== ガイドラインの背景と基本原則

そもそもDockerとは、「アプリケーションをどこでも簡単に構築・移動・実行」するためのツールであり、この考え方はコマンド体系や各種の実装において、大切にされている考え方です。多くの方にとって想像しやすいのは、Dockerを使って素早く簡単に「実行」するところ。docker runというシンプルなコマンドを実行するだけで、様々なアプリケーションが簡単に動きます。これはすごい！

一方で、開発・移動という部分は、実行する元となるイメージ構築過程での話です。Dockerイメージを作るには「docker build」コマンドを用います。しかし、ここでイメージ容量が大きくなってしまいますと、「イメージの構築に時間がかかる」「テストに時間がかかる」「イメージの移動に時間がかかる」という厄介な問題が発生してしまいます。せっかくDockerコンテナ化したアプリケーションは簡単に実行できても、それにいたる経緯で地獄を見なくてはいけない状況というのは歓迎されるものではありません。

つまり、大事なのは、いかに「イメージ容量を小さくするか？」というのがDockerが日の目を見て以来、今日まで継続して発生してきた課題であり、これをいかに克服するかがまとめられたのが、ベストプラクティスです。そのため、イメージ・レイヤの数はできるだけ小さくするのが鉄則でした。

また、イメージ容量を小さくするのと同時に「キャッシュの有効活用」も非常に需要です。このキャッシュとは、Dockerイメージ・レイヤをキャッシュとして利用すると、高速にイメージを構築できる点です。Dockerは構築時、Dockerfileの各行で変更がない行の命令はキャッシュを利用しようとします。例えば、大きなファイルのダウンロードやコピー、あるいはコンパイルの実行など、比較的時間のかかるような処理も、キャッシュの活用により素早い構築なりテストが可能になります。

そしてキャッシュを単純に活用する例として、頻繁に出てくるのが「RUN」命令を１つにまとめる方法です。次のようなDockerfileがあるとします。

//emlist{
FROM ubuntu:18.04
RUN apt-get update
RUN apt-get install -y curl nginx
//}

一度docker buildを実行してしまえば、2行目の「apt-get update」にかかる時間が省略されるため、キャッシュを活用した素早いイメージ構築が可能です。しかし、ちょっと罠があるので御注意ください。3行目を書き換えても、2行目の古いパッケージ情報がキャッシュとして残り続けてしまうため、古い情報を意図せず保持し続けてしまうリスクがあります。これを避けるには、次のようにして「\」（バックスラッシュ）と「&&」（シェル上のコマンドで、前の命令の実行に成功したら、次の処理をする）を組み合わせられます。さらに、不要なパッケージ情報も削除しておくと良いでしょう。

//emlist{
FROM ubuntu:18.04
RUN apt-get update \
 && apt-get install -y curl nginx \
 && rm -rf /var/lib/apt/lists/*
//}

== BuildKitの有効化

Docker CE v18.09移行のバージョンでは、docker buildコマンド実行時の内部プログラムを、新しいBuildKit（ビルドキット）に切り替えられます。BuildKit（builder v2）とは、開発途上の構築エンジンです。これは従来の構築エンジンと異なり、並列かつ高速なイメージ構築が可能な特長があります。DockerCon SF19の発表@<fn>{sakura5}では、通常のビルドで２倍の速さ、キャッシュを有効化すると約7.2倍になる場合もあるという事例が紹介されていました。

有効化するには、dockerコマンドを使うクライアント側の環境変数で、次の1行を実行するだけです。

//emlist{
export DOCKER_BUILDKIT=1
//}

あとは、docker buildコマンドを実行するだけで、新しいバージョンとしての実行がされます。実行しますと、表示結果画面も従来と異なりますので、速さを体感だけでなく、ちょっと新鮮な感覚も味わえると思います。なお、ログアウトすると元に戻ってしいますので、常用する場合は~/.bashrcに記載しておくなど、環境変数上で有効化する作業をしておきましょう。

//footnote[sakura5][Dockerfile Best Practices https://www.slideshare.net/Docker/dcsf19-dockerfile-best-practices/52]

== その他、最新のベストプラクティス

今回紹介したベストプラクティスは基本中の基本です。より高速かつ効率的なイメージ構築のためには、そのほかのベストプラクティスにも慣れていきましょう。実は今回の原稿の元ネタとなったスライドは、既にインターネットで公開しています。もう少しDockerfileと「友達」になるべく、以下のスライドを御覧ください。

 * Dockerfileを書くためのベストプラクティス解説編
 ** https://www.slideshare.net/zembutsu/explaining-best-practices-for-writing-dockerfiles
 * Dockerfileを改善するためのBest Practice 2019年度版
 ** https://www.slideshare.net/zembutsu/dockerfile-bestpractices-19-and-advice

== まとめ

Dockerfileは、Dockerイメージを自動的に構築するための元になるファイルです。これを使ってdocker buildコマンドを実行するとイメージを自動構築できます。ただし、Dockerfileの書き方によっては、構築に時間がかかり、逆に日常の非効率につながりかねません。適切にDockerfileを書くことにより、Dockerイメージの構築が効率化するだけでなく、開発やテストなども素早く効率的に行えるようになります。慣れないうちは面倒だとお感じになるかもしれません。ですが、覚えることはそんなに多くありませんので、手作業で変更をするよりはDockerfileで効率的に開発なり検証を進められるように頑張っていきましょう。あと、BuildKitの有効化については是非、お試しいただいて常用いただければと思っています。
