# ������� ��� ������ ��������� �� ������� � ���������� �������
function abort {
    echo "������: $1" >&2
    exit 1
}

# �������� ���������� ������ ����� Makefile
make clean || abort "�� ������� �������� ���������� ������."

# ������ ������� � �������������� Makefile
make daemonLab || abort "�� ������� ������� ������."

echo "������ ��������� �������."

# �������� ������� ����������������� ����� � ���������� ��� PID-�����
CONFIG_FILE="./config.txt"
PID_DIR=$(dirname "$CONFIG_FILE")

if [[ ! -f $CONFIG_FILE ]]; then
    abort "���������������� ���� �����������: $CONFIG_FILE"
fi

if [[ ! -d $PID_DIR ]]; then
    abort "���������� ��� PID-����� �����������: $PID_DIR"
fi

# ������ ������
echo "��������� ������..."
sudo ./daemonLab "$CONFIG_FILE" "$PID_DIR/daemon.pid" || abort "�� ������� ��������� ������."

echo "����� ������� �������."

# ������ �� �������� ������������ �����
read -p "������ ������� ����������� ����? (y/n): " response
if [[ $response == "y" ]]; then
    sudo rm -f ./daemonLab || abort "�� ������� ������� ����������� ����."
    echo "����������� ���� �����."
else
    echo "����������� ���� �������."
fi