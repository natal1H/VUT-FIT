-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (FAIL, TEST1, TEST2, TEST3, TEST4, TEST5, TEST6a, TEST7a,
   TEST8a, TEST9a, TEST10a, TEST6b, TEST7b, TEST8b, TEST9b, TEST10b, SUCCESS, PRINT_SUCCESS_MSG, PRINT_FAIL_MSG, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
      when FAIL =>
      next_state <= FAIL;
      if (KEY(15) = '1') then
        next_state <=PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST1 =>
      next_state <= TEST1;
      if (KEY(1) = '1') then
        next_state <= TEST2;		
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST2 =>
        next_state <= TEST2;
      if (KEY(1) = '1') then
        next_state <= TEST3; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST3 =>
      next_state <= TEST3;
      if (KEY(9) = '1') then
        next_state <= TEST4;  
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;     
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST4 =>
      next_state <= TEST4;
      if (KEY(5) = '1') then
        next_state <= TEST5;     
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST5 =>
      next_state <= TEST5;
      if (KEY(9) = '1') then
        next_state <= TEST6a;   
	  elsif (KEY(0) = '1') then
        next_state <= TEST6b; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;    
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST6a =>
      next_state <= TEST6a; 
      if (KEY(1) = '1') then
        next_state <= TEST7a;  
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;     
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST7a =>
      next_state <= TEST7a;
      if (KEY(2) = '1') then
        next_state <= TEST8a;    
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST8a =>
      next_state <= TEST8a;
      if (KEY(9) = '1') then
        next_state <= TEST9a;    
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST9a =>
      next_state <= TEST9a;
      if (KEY(9) = '1') then
        next_state <= TEST10a;  
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST10a =>
      next_state <= TEST10a;
      if (KEY(2) = '1') then
        next_state <= SUCCESS; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST6b =>
      next_state <= TEST6b;
      if (KEY(9) = '1') then
        next_state <= TEST7b;    
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST7b =>
      next_state <= TEST7b;
      if (KEY(5) = '1') then
        next_state <= TEST8b;  
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST8b =>
      next_state <= TEST8b;
      if (KEY(5) = '1') then
        next_state <= TEST9b; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST9b =>
      next_state <= TEST9b;
      if (KEY(9) = '1') then
        next_state <= TEST10b; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;     
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST10b =>
      next_state <= TEST10b;
      if (KEY(0) = '1') then
        next_state <= SUCCESS;  
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;   
      elsif (KEY(15) = '1') then
        next_state <= PRINT_FAIL_MSG;
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when SUCCESS =>
      next_state <= SUCCESS;
      if (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_SUCCESS_MSG;     
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL_MSG =>
      next_state <= PRINT_FAIL_MSG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUCCESS_MSG =>
      next_state <= PRINT_SUCCESS_MSG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUCCESS_MSG =>
      FSM_CNT_CE     <= '1';
      FSM_MX_MEM     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL_MSG =>
      FSM_CNT_CE     <= '1';
      FSM_MX_MEM     <= '0';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   if (KEY(14 downto 0) /= "000000000000000") then
      FSM_LCD_WR     <= '1';
   end if;
   if (KEY(15) = '1') then
      FSM_LCD_CLR    <= '1';
   end if;  
   end case;
end process output_logic;

end architecture behavioral;


