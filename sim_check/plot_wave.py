#!/usr/bin/env python
# coding: utf-8
# 解析 ModelSim VCD (tb_reg1.vcd, 真实 IP OUT_REG=1)，绘制数字时序图。
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import matplotlib.font_manager as fm

# 中文字体（Windows）
for f in ['Microsoft YaHei','SimHei','DengXian']:
    try:
        fm.findfont(f, fallback_to_default=False); plt.rcParams['font.sans-serif']=[f]; break
    except Exception: pass
plt.rcParams['axes.unicode_minus']=False

def parse_vcd(path):
    scope=[]; idinfo={}
    lines=open(path).read().splitlines()
    body_start=0
    for k,ln in enumerate(lines):
        if ln.startswith('$scope'): scope.append(ln.split()[2])
        elif ln.startswith('$upscope'):
            if scope: scope.pop()
        elif ln.startswith('$var'):
            p=ln.split(); size=int(p[2]); ident=p[3]; name=p[4]
            bit=p[5] if (len(p)>5 and p[5]!='$end') else None
            idinfo[ident]=('/'.join(scope)+'/'+name,bit,size)
        elif ln.startswith('$enddefinitions'):
            body_start=k+1; break
    events={}; t=0
    for ln in lines[body_start:]:
        if not ln: continue
        if ln[0]=='#': t=int(ln[1:]); continue
        c=ln[0]
        if c in '01xXzZ': val=ln[0]; ident=ln[1:]
        elif c in 'bBrR': sp=ln.split(); val=sp[0][1:]; ident=sp[1]
        else: continue
        events.setdefault(ident,[]).append((t,val))
    return idinfo, events

def is_vector(bit):  # 范围[a:b]或None -> 向量；单bit[a] -> 拆分位
    return bit is None or (bit and ':' in bit)

def bus_series(idinfo, events, busname):
    vec_id=None; bits={}; width=0
    for ident,(full,bit,sz) in idinfo.items():
        if full!=busname: continue
        if is_vector(bit): vec_id=ident; width=sz
        else:
            bi=int(bit.strip('[]')); bits[bi]=ident; width=max(width,bi+1)
    if vec_id is not None:
        out=[]
        for t,v in sorted(events.get(vec_id,[])):
            iv=int(v,2) if v and all(ch in '01' for ch in v) else None
            out.append((t,iv))
        return out
    cur={bi:'x' for bi in bits}; allev=[]
    for bi,ident in bits.items():
        for t,v in events.get(ident,[]): allev.append((t,bi,v))
    allev.sort(); out=[]
    for t,bi,v in allev:
        cur[bi]=v
        s=''.join(cur.get(b,'x') for b in range(width-1,-1,-1))
        iv=int(s,2) if all(ch in '01' for ch in s) else None
        if out and out[-1][0]==t: out[-1]=(t,iv)
        else: out.append((t,iv))
    return out

def scalar_series(idinfo, events, signame):
    for ident,(full,bit,sz) in idinfo.items():
        if full==signame and is_vector(bit): return sorted(events.get(ident,[]))
    return []

def val_at(series, t0):
    v=None
    for t,val in series:
        if t<=t0: v=val
        else: break
    return v

def window(series, t0, t1):
    """返回窗口内的 (t,val) 段列表，含 t0 处的初值。"""
    res=[(t0, val_at(series,t0))]
    for t,val in series:
        if t0 < t <= t1: res.append((t,val))
    return res

NS=1000.0  # ps->ns

def draw_bit(ax, y, seg, t0, t1, label, color='#1f77b4'):
    H=0.62
    xs=[]; ys=[]
    pts=seg+[(t1,seg[-1][1])]
    for i in range(len(pts)-1):
        t,v=pts[i]; tn=pts[i+1][0]
        lvl = H if v=='1' else (0.0 if v=='0' else H/2)
        x0,x1=t/NS,tn/NS
        if xs and v in '01':
            # 垂直跳变
            ax.plot([x0,x0],[ys[-1],lvl+y],color=color,lw=1.3,zorder=3)
        if v in '01':
            ax.plot([x0,x1],[lvl+y,lvl+y],color=color,lw=1.3,zorder=3)
        else:  # x/z
            ax.add_patch(plt.Rectangle((x0,y),x1-x0,H,facecolor='#ffd9d9',edgecolor='none',zorder=1))
            ax.plot([x0,x1],[y+H/2,y+H/2],color='#cc0000',lw=1.0,ls='--',zorder=3)
        xs.append(x0); ys.append(lvl+y)
    ax.text(t0/NS-0.02*(t1-t0)/NS, y+H/2, label, ha='right', va='center', fontsize=8.5)

def draw_bus(ax, y, seg, t0, t1, label, fmt=lambda v:f"{v:X}"):
    H=0.62
    pts=seg+[(t1,seg[-1][1])]
    for i in range(len(pts)-1):
        t,v=pts[i]; tn=pts[i+1][0]
        x0,x1=t/NS,tn/NS
        if v is None:  # 高阻/未知
            ax.add_patch(plt.Rectangle((x0,y),x1-x0,H,facecolor='#eeeeee',edgecolor='#bbbbbb',zorder=1))
            ax.text((x0+x1)/2,y+H/2,'Z',ha='center',va='center',fontsize=7,color='#888')
            continue
        c='#dceaf7'
        poly=Polygon([(x0+0.4,y),(x1-0.4,y),(x1,y+H/2),(x1-0.4,y+H),(x0+0.4,y+H),(x0,y+H/2)],
                     closed=True, facecolor=c, edgecolor='#2c5f8a', lw=1.0, zorder=2)
        ax.add_patch(poly)
        if (x1-x0) > (t1-t0)/NS*0.012:
            ax.text((x0+x1)/2,y+H/2,fmt(v),ha='center',va='center',fontsize=7.5,zorder=4)
    ax.text(t0/NS-0.02*(t1-t0)/NS, y+H/2, label, ha='right', va='center', fontsize=8.5)

def make_fig(specs, t0, t1, title, fname, marks=None):
    n=len(specs)
    fig,ax=plt.subplots(figsize=(13, 0.62*n+1.3))
    for k,(label,kind,series,extra) in enumerate(specs):
        y=(n-1-k)*1.0
        seg=window(series,t0,t1)
        if kind=='bit': draw_bit(ax,y,seg,t0,t1,label,color=extra or '#1f77b4')
        else: draw_bus(ax,y,seg,t0,t1,label,fmt=extra or (lambda v:f"{v:X}"))
    if marks:
        for mt,mlabel in marks:
            ax.axvline(mt/NS,color='#e08000',ls=':',lw=1.0,zorder=0)
            ax.text(mt/NS,n*1.0-0.15,mlabel,rotation=90,fontsize=7,color='#b06000',va='top',ha='right')
    ax.set_xlim(t0/NS - (t1-t0)/NS*0.10, t1/NS)
    ax.set_ylim(-0.3, n*1.0+0.2)
    ax.set_yticks([])
    ax.set_xlabel('time (ns)')
    ax.set_title(title, fontsize=11, fontweight='bold')
    for s in ['top','left','right']: ax.spines[s].set_visible(False)
    ax.grid(axis='x', ls=':', alpha=0.4)
    plt.tight_layout()
    fig.savefig(fname, dpi=130)
    print("saved", fname)

if __name__=='__main__':
    idinfo,events=parse_vcd('tb_reg1.vcd')
    S =lambda n: scalar_series(idinfo,events,n)
    B =lambda n: bus_series(idinfo,events,n)
    clk130=S('tb/clk130'); cs=S('tb/cs'); rs=S('tb/rs'); wr=S('tb/wr'); rd=S('tb/rd')
    SD=B('tb/SD'); ADDR=B('tb/dut/fmc_inst/ADDR'); RDD=B('tb/dut/fmc_inst/RD_DATA')
    WA2=S('tb/dut/fmc_inst/WR_ADDR2'); WA3=S('tb/dut/fmc_inst/WR_ADDR3')
    FDET=B('tb/dut/fmc_inst/FMC_DET'); canr=S('tb/dut/can_read')
    snum=B('tb/dut/sample_num'); FOUT=B('tb/dut/FIFO_OUT')
    IND=B('tb/dut/my_fifo_inst/IN_DATA'); wrreq=S('tb/dut/my_fifo_inst/wrreq')
    wrfull=S('tb/dut/my_fifo_inst/wrfull'); wdone=S('tb/dut/my_fifo_inst/write_done')
    wrcnt=B('tb/dut/my_fifo_inst/wr_count'); rdreq=S('tb/dut/my_fifo_inst/rdreq')

    dec=lambda v:f"{v}"
    # 图1：FIFO 写入（130MHz 写时钟域）
    make_fig([
        ('clk130','bit',clk130,'#444'),
        ('wrreq','bit',wrreq,'#1f77b4'),
        ('IN_DATA[15:0]','bus',IND,None),
        ('wr_count','bus',wrcnt,dec),
        ('wrfull','bit',wrfull,'#1f77b4'),
        ('write_done','bit',wdone,'#d62728'),
    ], 95000, 290000, '图1  FIFO 写入阶段（ADC→FIFO，130MHz 写时钟域）首帧16样本', 'wave1_fifo_write.png')

    # 图2：轮询 ready 标志
    make_fig([
        ('CS','bit',cs,'#444'),('RS(A12)','bit',rs,'#9467bd'),
        ('WR','bit',wr,'#2ca02c'),('RD','bit',rd,'#ff7f0e'),
        ('SD[15:0]','bus',SD,dec),('ADDR(reg)','bus',ADDR,dec),
        ('FMC_DET','bus',FDET,dec),('can_read','bit',canr,'#d62728'),
    ], 900000, 1500000, '图2  STM32 轮询 ready 标志（写地址0x01→读，直到读回255）', 'wave2_poll_ready.png',
       marks=[(1268000,'can_read↑')])

    # 图3：数据读循环（num/data 对齐，关键）
    make_fig([
        ('CS','bit',cs,'#444'),('RS(A12)','bit',rs,'#9467bd'),
        ('WR','bit',wr,'#2ca02c'),('RD','bit',rd,'#ff7f0e'),
        ('SD[15:0]','bus',SD,None),('ADDR(reg)','bus',ADDR,dec),
        ('WR_ADDR2','bit',WA2,'#17becf'),('WR_ADDR3','bit',WA3,'#bcbd22'),
        ('rdreq','bit',rdreq,'#1f77b4'),('sample_num','bus',snum,dec),
        ('FIFO_OUT','bus',FOUT,None),('RD_DATA','bus',RDD,None),
    ], 1460000, 2380000, '图3  数据读循环：写0x02读num→写0x03读data（num与data严格对齐，OUT_REG=1）', 'wave3_data_read.png')
